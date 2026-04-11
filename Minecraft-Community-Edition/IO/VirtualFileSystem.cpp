/*
 * VirtualFileSystem.cpp
 *
 * Implementation notes and high level overview:
 *
 * - In debug builds the VFS is represented by a JSON mapping (data) where
 *   each key is a tag and each value is a file path string. loadJSONMappingFile
 *   reads that JSON mapping from disk into `data`.
 *
 * - buildJSONMappingFile consumes the JSON mapping and writes a single
 *   binary .VFS file with a simple header and a content table, then appends
 *   every referenced file's raw bytes into the output .VFS file.
 *
 * - In release builds the binary .VFS files are loaded. The loader expects
 *   a magic 32-bit value equal to the ASCII ".VFS" (0x5346562e) then:
 *     - 1 byte fileType
 *     - 3 bytes version (major, minor, patch)
 *     - 8 bytes contentHeaderSize (u64)
 *   The content header then contains repeated entries of:
 *     - NUL-terminated tag name (variable length)
 *     - 8 bytes start offset (u64)
 *     - 8 bytes end offset (u64)
 *
 *   When cfg.useStreamInput is true the FileInputStream is used to read
 *   contents on demand. Otherwise the .VFS payload is loaded entirely into
 *   memory (binary) and reads are memcpy'ed out of that buffer.
 *
 */

#include "VirtualFileSystem.hpp"
#include "Stream/FileOutputStream.hpp"
#include "Stream/MemoryInputStream.hpp"

namespace mce::io {
	using stream::FileOutputStream;
	using stream::MemoryInputStream;

/* Create a packed Version from components. */
VirtualFileSystem::Version VirtualFileSystem::Version::makeVersion(unsigned char major, unsigned char minor, unsigned char patch) {
	return { major, minor, patch };
}

#ifdef _DEBUG

/*
 * Debug-only: read a JSON mapping file from disk and parse it into `data`.
 * The function opens the path using FileInputStream, reads the file bytes
 * into a temporary buffer, and uses nlohmann::json::parse().
 *
 * Returns true on success.
 */
bool VirtualFileSystem::loadJSONMappingFile(const std::string& fileName) {
	FileInputStream file;
	if (file.open(fileName.data())) {
		size_t size = file.getSize();

		/* FILE_NOT_FOUND and other errors are represented by INVALID_FILE_SIZE (~0) */
		if (size != ~(0)) {
			/* allocate temporary buffer with room for NUL terminator required by json parser */
			char* buffer = new char[size + 1] {};

			file.read(buffer, size);

			/* parse and store mapping into the debug `data` JSON object */
			VirtualFileSystem::data = nlohmann::json::parse(buffer);

			delete[] buffer;
			return true;
		}
	}
	return false;
}
#endif

/*
 * Destructor: clean up resources allocated in release mode.
 * - close the fileInStream if used
 * - free the binary buffer if the VFS payload was loaded to memory
 */
VirtualFileSystem::~VirtualFileSystem() {
#ifndef _DEBUG
	VirtualFileSystem::fileInStream.close();
	if (VirtualFileSystem::binary != nullptr) {
		delete[] VirtualFileSystem::binary;
		VirtualFileSystem::binary = nullptr;
	}
#endif
}

/* Return reference to configuration (convenience wrapper). */
VirtualFileSystem::Config& VirtualFileSystem::getConfig() {
	return this->cfg;
}

/*
 * Retrieve file data by tag.
 *
 * - In debug builds: iterate the JSON mapping, open the referenced file path and
 *   read the bytes into the provided `bytes` vector.
 *
 * - In release builds, two modes:
 *   1) cfg.useStreamInput == true: use fileInStream to seek and read the bytes
 *      directly from the .VFS file on disk.
 *   2) cfg.useStreamInput == false: copy bytes from the in-memory `binary`
 *      payload into the provided vector.
 *
 * The function resizes `bytes` when necessary. Returns true if the tag was
 * found and bytes were loaded successfully.
 */
bool VirtualFileSystem::getFile(const eastl::string_view& fileTag, eastl::vector<uint8_t>& bytes) {
#ifdef _DEBUG
	for (auto& file : VirtualFileSystem::data.items()) {
		if (file.key() == fileTag.data()) {
			if (file.value().is_string()) {
				FileInputStream fIn;
				if (fIn.open(file.value())) {
					size_t size = fIn.getSize();
					if (size != ~(0)) {
						if (bytes.size() >= size) {
							fIn.read(bytes.data(), size);
						}
						else {
							bytes.resize(size);
							fIn.read(bytes.data(), bytes.size());
						}
						return true;
					}
				}
			}
		}
	}
	return false;
#else
	/* Release build path: streaming or in-memory access. */
	if (VirtualFileSystem::cfg.useStreamInput) {
		if (VirtualFileSystem::files.count(fileTag.data())) {
			const auto& file = VirtualFileSystem::files[fileTag.data()];
			/* seek to file start inside the .VFS and read the requested bytes */
			fileInStream.seek(file.start);

			if (bytes.size() >= file.size) {
				fileInStream.read(bytes.data(), file.size);
			}
			else {
				/* ensure vector capacity and read */
				bytes.resize(file.size, 0);
				fileInStream.read(bytes.data(), bytes.size());
			}
			return true;
		}
	}
	else {
		/* copy from in-memory binary payload */
		if (VirtualFileSystem::files.count(fileTag.data())) {
			const auto& file = VirtualFileSystem::files[fileTag.data()];
			if (bytes.size() >= file.size) {
				std::memcpy(bytes.data(), VirtualFileSystem::binary + (file.start), file.size);
			}
			else {
				bytes.resize(file.size);
				std::memcpy(bytes.data(), VirtualFileSystem::binary + (file.start), file.size);
			}
			return true;
		}
	}
	return false;
#endif
}

/*
 * buildJSONMappingFile
 *
 * - Reads a JSON mapping describing tag -> path relationships.
 * - Computes an aggregated content header size describing all entries.
 * - Writes a binary .VFS file that contains:
 *     - 4 bytes signature ".VFS"
 *     - 1 byte fileType
 *     - 3 bytes version (major/minor/patch)
 *     - 8 bytes contentHeaderSize (u64)
 *     - content header entries (tag NUL-terminated + start + end for each)
 *     - raw concatenation of the referenced file contents in the same order
 *
 * The method populates VirtualFileSystem::files with metadata for each tag
 * while computing offsets. Returns true on success.
 */
bool VirtualFileSystem::buildJSONMappingFile(const std::string& fileInName, const std::string& fileOutName) {
	FileInputStream fIn;
	nlohmann::json data;
	if (fIn.open(fileInName.data())) {
		size_t size = fIn.getSize();

		if (size != ~(0)) {
			char* buffer = new char[size + 1] {};

			fIn.read(buffer, size);

			/* parse mapping json */
			data = nlohmann::json::parse(buffer);

			if (data.is_discarded()) {
				// the file isn't json or contains syntax error
				return false;
			}

			delete[] buffer;
			buffer = nullptr;

			/* determine size of the content header by inspecting each mapping entry */
			size_t contentHeaderSize = 0;
			for (auto& file : data.items()) {
				if (file.value().is_string()) {
					FileInputStream _fileIn;

					if (_fileIn.open(file.value())) {
						VirtualFileSystem::File _file;

						_file.path = file.value();
						_file.size = _fileIn.getSize();

						/* tag length + offset (8) + size (8) + 1 for trailing separator/NUL */
						contentHeaderSize += file.key().size() + 8 /*offset*/ + 8 /*size*/ + 1/*for the next element*/;
						VirtualFileSystem::files[file.key().c_str()] = _file;
					}
					else {
						return false;
					}
				}
			}

			/* fileCursor determines the start offset of the first file's payload.
			   The initial 32 bytes and adjustments are historic/layout-specific
			   to this implementation. The math below preserves existing behavior. */
			size_t fileCursor = 32;
			fileCursor += contentHeaderSize - 16;

			/* create the output .VFS and write header + content table */
			FileOutputStream fOut;
			if (fOut.open(fileOutName)) {

				/* 4-byte signature ".VFS" */
				fOut.write(".VFS", 4);

				/* fileType and version bytes */
				fOut.write(&VirtualFileSystem::cfg.fileType, 1);
				fOut.write(&VirtualFileSystem::cfg.version.major, 1);
				fOut.write(&VirtualFileSystem::cfg.version.minor, 1);
				fOut.write(&VirtualFileSystem::cfg.version.patch, 1);

				/* write content header size (8 bytes) */
				fOut.write(&contentHeaderSize, 8);

				/* write content header (tag + start + end for each file) */
				for (auto& file : VirtualFileSystem::files) {
					/* tag zero terminated */
					eastl::string tagName = file.first;
					tagName.push_back(0);
					fOut.write(tagName.c_str(), tagName.length());

					/* 8-byte start offset */
					file.second.start = fileCursor;
					fOut.write(&file.second.start, 8);

					/* 8-byte end offset (inclusive) */
					file.second.end = file.second.start + file.second.size - 1;
					fOut.write(&file.second.end, 8);

					/* advance cursor to end+1 (start of next file) */
					fileCursor = file.second.end;
					fileCursor++;
				}

				/* append raw file payloads in the same order as the table above */
				for (auto& file : VirtualFileSystem::files) {
					FileInputStream _fileIn;
					if (_fileIn.open(file.second.path)) {
						size_t _size = _fileIn.getSize();
						if (_size != ~(0)) {
							char* _buffer = new char[_size];
							_fileIn.read(_buffer, _size);

							fOut.write(_buffer, _size);
							delete[] _buffer;
							_buffer = nullptr;
						}
					}
					else {
						return false;
					}
				}
			}
			fOut.close();
		}
	}

	return true;
}

/*
 * Convenience wrapper to pick debug JSON loader or release binary loader
 * depending on build configuration.
 */
bool VirtualFileSystem::loadFile(const std::string& fileName) {
#ifdef _DEBUG
	return VirtualFileSystem::loadJSONMappingFile(fileName + ".json");
#else
	return VirtualFileSystem::loadVirtualFileSystem(fileName + ".bin");
#endif
}

#ifndef _DEBUG
/*
 * release-mode loader: open a .VFS file and build the internal table.
 *
 * Steps:
 * - Validate the magic signature (.VFS)
 * - Read fileType and version bytes
 * - Read contentHeaderSize and ensure it's reasonable (guard with short max)
 * - Read the content header into a temporary buffer
 * - Parse entries using MemoryInputStream which reads NUL-terminated tags and numeric offsets
 * - When useStreamInput is false, load the remaining payload into memory (`binary`) for random access
 *
 * The function returns true on success and false where errors or validation
 * failures occur.
 */
bool VirtualFileSystem::loadVirtualFileSystem(const std::string& fileName) {
	if (VirtualFileSystem::cfg.useStreamInput) {
		if (VirtualFileSystem::fileInStream.open(fileName)) {
			int vfsName = 0;
			VirtualFileSystem::fileInStream.read(&vfsName, sizeof(vfsName));
			/* magic number for ".VFS" */
			if (vfsName == 0x5346562e) {
				
				/* read file type */
				VirtualFileSystem::fileInStream.read(&cfg.fileType, sizeof(VirtualFileSystem::FileBinaryType));

				/* read version bytes */
				VirtualFileSystem::fileInStream.read(&cfg.version.major, sizeof(char));
				VirtualFileSystem::fileInStream.read(&cfg.version.minor, sizeof(char));
				VirtualFileSystem::fileInStream.read(&cfg.version.patch, sizeof(char));

				/* content header size is stored as size_t (8 bytes on typical platforms) */
				size_t contentHeaderSize = 0;
				VirtualFileSystem::fileInStream.read(&contentHeaderSize, sizeof(size_t));
				/* historic file format stores contentHeaderSize with an extra marker; decrement to get usable length */
				contentHeaderSize--; // Decrement with one 

				/* simple guard to avoid allocating extremely large buffers */
				short maxContentHeaderSize = ~(0i16);

				if (contentHeaderSize <= maxContentHeaderSize) {
					/* read content header into a temporary buffer */
					char* buffer = new char[contentHeaderSize] {};

					if (buffer == nullptr) {
						return false; // allocation error
					}

					VirtualFileSystem::fileInStream.read(buffer, contentHeaderSize);

					/* use MemoryInputStream to parse NUL-terminated tags and offsets from the header */
					MemoryInputStream memIn(buffer, contentHeaderSize);

					int i = 0;
					/* iterate the header until processed length reaches contentHeaderSize */
					while (i <= contentHeaderSize) {
						size_t startLocation = 0, endLocation = 0;

						char* tagName = nullptr;
						size_t tagNameSize = 0;
						/* readString returns bytes consumed; ~(0) indicates failure */
						if (memIn.readString(tagName, tagNameSize) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the tagName;
						}

						if (memIn.read(&startLocation, sizeof(size_t)) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the startLocation;
						}

						if (memIn.read(&endLocation, sizeof(size_t)) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the endLocation;
						}

						/* store metadata for this tag */
						VirtualFileSystem::File _file;
						_file.start = startLocation;
						_file.end = endLocation;
						_file.size = _file.end - _file.start;

						VirtualFileSystem::files[tagName] = _file;

						/* advance processed counter by tag length + offsets */
						i += tagNameSize + 1 /*the zero terminated byte*/ + sizeof(startLocation) + sizeof(endLocation);
					}
				}
				else {
					/* content header unreasonably large */
					return false;
				}
				
			}
			else {
				/* not a VFS file */
				return false;
			}
		}
	}
	else {
		/* Non-streaming path: behaviour is similar but after parsing the header we
		   load the remaining file payload into memory (binary) for direct access. */
		if (VirtualFileSystem::fileInStream.open(fileName)) {
			int vfsName = 0;
			VirtualFileSystem::fileInStream.read(&vfsName, sizeof(vfsName));
			if (vfsName == 0x5346562e) {

				VirtualFileSystem::fileInStream.read(&cfg.fileType, sizeof(VirtualFileSystem::FileBinaryType));
				VirtualFileSystem::fileInStream.read(&cfg.version.major, sizeof(char));
				VirtualFileSystem::fileInStream.read(&cfg.version.minor, sizeof(char));
				VirtualFileSystem::fileInStream.read(&cfg.version.patch, sizeof(char));

				size_t contentHeaderSize = 0;
				VirtualFileSystem::fileInStream.read(&contentHeaderSize, sizeof(size_t));
				contentHeaderSize--; // Decrement with one 

				short maxContentHeaderSize = ~(0i16);

				if (contentHeaderSize <= maxContentHeaderSize) {
					char* buffer = new char[contentHeaderSize] {};

					if (buffer == nullptr) {
						return false; // allocation error
					}

					VirtualFileSystem::fileInStream.read(buffer, contentHeaderSize);

					MemoryInputStream memIn(buffer, contentHeaderSize);

					int i = 0;
					while (i <= contentHeaderSize) {
						size_t startLocation = 0, endLocation = 0;

						char* tagName = nullptr;
						size_t tagNameSize = 0;
						if (memIn.readString(tagName, tagNameSize) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the tagName;
						}

						if (memIn.read(&startLocation, sizeof(size_t)) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the startLocation;
						}

						if (memIn.read(&endLocation, sizeof(size_t)) == ~(0)) {
							delete[] buffer;
							return false; // error on reading the endLocation;
						}

						VirtualFileSystem::File _file;
						/* adjust offsets to map into the in-memory payload layout */
						_file.start = startLocation - (contentHeaderSize + 16 /* header + signature */);
						_file.end = endLocation - (contentHeaderSize + 16);
						_file.size = endLocation - startLocation;

						VirtualFileSystem::files[tagName] = _file;

						i += tagNameSize + 1 /*the zero terminated byte*/ + sizeof(startLocation) + sizeof(endLocation);
					}

					/* compute remaining payload size, allocate and read it into memory */
					size_t fileDataSize = VirtualFileSystem::fileInStream.getSize() - (contentHeaderSize + 8 /*vfs name and the header*/);

					VirtualFileSystem::binary = new char[fileDataSize];

					if (VirtualFileSystem::binary == nullptr) {
						delete[] buffer;
						buffer = nullptr;
						return false; // allocation error
					}

					/* read the concatenated payload into `binary` */
					fileInStream.read(VirtualFileSystem::binary, fileDataSize);

				}
				else {
					/* header too large */
					return false;
				}

			}
			else {
				/* not a VFS file */
				return false;
			}
		}
	}

	return false;
}
#endif

} // namespace mce

