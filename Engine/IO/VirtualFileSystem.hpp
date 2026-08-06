#pragma once
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#define JSON_NOEXCEPTION
#include "nlohmann/json.hpp"
#include "Stream/FileInputStream.hpp"

namespace bgfx {
	struct Memory;
}

namespace dms::io {
	using stream::FileInputStream;
/**
 * @brief VirtualFileSystem manages a collection of files referenced by tags.
 *
 * The virtual file system can be built from a JSON mapping (debug mode) or
 * from a binary VFS file (release). It supports loading file contents either
 * via stream input or by keeping the virtual file contents in memory.
 */
	class VirtualFileSystem {
	public:
		~VirtualFileSystem();
		/**
		 * @brief Types of virtual file collections that can be stored in a VFS file.
		 */
		enum FileBinaryType : unsigned char {
			NONE = 0,       /**< No file type specified. */
			ASSETS_FILE = 1,/**< Assets bundle. */
			SOURCE_FILE = 2,/**< Source files bundle. */
			WORLD_FILE  = 4 /**< World data bundle. */
		};

		/**
		 * @brief Version information for the virtual file system format.
		 *
		 * The Version is a small union allowing access either to individual
		 * components or as a 32-bit quad value.
		 */
		union Version {

			/**
			 * @brief Create a Version from components.
			 * @param major Major version.
			 * @param minor Minor version.
			 * @param patch Patch version.
		 */
			static Version makeVersion(unsigned char major, unsigned char minor, unsigned char patch);

			struct {
				unsigned char major, minor, patch; /**< Version components. */
			};

			unsigned int quad; /**< Packed 32-bit representation of the version. */
		};

		/**
		 * @brief Configuration for how the VirtualFileSystem should operate.
		 */
		struct Config {
			bool useStreamInput = false; /**< If true, files are accessed via streams; otherwise VFS binary may be used in-memory. */
			FileBinaryType fileType = FileBinaryType::NONE; /**< Type of stored file collection. */
			Version version = { 0 }; /**< Version of the VFS format to expect. */
		};

		/**
		 * @brief Access the runtime configuration.
		 * @return Reference to the VFS configuration.
		 */
		Config& getConfig();

		/**
		 * @brief Retrieve a file's bytes identified by its tag.
		 * @param fileTag Tag identifying the file in the VFS.
		 * @param bytes Output vector which will be filled with file data.
		 * @return True if the file was found and bytes were successfully loaded.
		 */
		bool getFile(const eastl::string_view& fileTag, eastl::vector<uint8_t>& bytes);

		bool getFile(const eastl::string_view& fileTag, bgfx::Memory*& mem);

		// it must be nullptr
		// Example: void* buffer = nullptr;
		bool getFile(const eastl::string_view& fileTag, void*& buffer, size_t& size);

		/**
		 * @brief Build a binary VFS file from a JSON mapping.
		 * @param fileInName Path to the input JSON mapping file.
		 * @param fileOutName Path where the resulting binary VFS should be written.
		 * @return True if the output file was successfully created.
		 */
		bool buildJSONMappingFile(const std::string& fileInName, const std::string& fileOutName);
		
		/**
		 * @brief Load the virtual file system from a file.
		 * @param fileName Base file name (without extension) to load.
		 * @return True if the VFS data was successfully loaded.
		 */
		bool loadFile(const std::string& fileName);

		bool findFileTag(const std::string& fTag);
	private:
#ifdef _DEBUG
		/**
		 * @brief Load mapping from a JSON file (debug builds).
		 * @param fileName Path to the JSON mapping file.
		 * @return True on success.
		 */
		bool loadJSONMappingFile(const std::string& fileName);
#else
		/**
		 * @brief Load the binary virtual file system (release builds).
		 * @param fileName Path to the binary VFS file.
		 * @return True on success.
		 */
		bool loadVirtualFileSystem(const std::string& fileName);
#endif
		/**
		 * @brief Internal representation of a single file in the VFS.
		 */
		struct File {
			std::string path; /**< Original path to the source file. */
			size_t start, end; /**< Byte offsets (inclusive) inside the VFS data. */

			size_t size; /**< Computed size of the file in bytes. */
		};

		/**
		 * @brief Map from file tag to File metadata.
		 *
		 * The key is the tag string used to lookup files in the VFS.
		 */
		eastl::unordered_map<eastl::string, File> files;

		Config cfg; /**< Runtime configuration for the VFS. */
#ifdef _DEBUG
		nlohmann::json data; /**< JSON mapping data used in debug mode. */
#else
		FileInputStream fileInStream; /**< Stream used for reading VFS content from disk. */
		/**
		 * @brief When not using streaming input this points to the loaded VFS binary in memory.
		 *
		 * The buffer is owned by the VirtualFileSystem and should be freed by it.
		 */
		char* binary; 
#endif
	};
}
