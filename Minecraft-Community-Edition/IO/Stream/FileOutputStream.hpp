#pragma once
#include "..\..\Common\Platform.hpp"
#include "OutputStream.hpp"
#ifdef MCE_PLATFORM_WINDOWS
#undef INVALID_FILE_SIZE
#endif

#include <string>
#include <cstdio>

namespace mce {
/**
 * @brief Platform-independent file output stream implementation.
 *
 * Provides a concrete OutputStream that writes data to a file using the
 * platform native file handle or FILE* depending on the platform.
 */
	class FileOutputStream final : public OutputStream {
	public:
		/**
		 * @brief Default constructor.
		 */
		FileOutputStream() = default;

		/**
		 * @brief Opens a file for writing (creates/truncates by default).
		 * @param file Path (absolute or relative) to the file to open.
		 * @return True if the file was successfully opened.
		 */
		bool open(const std::string& file);

		/**
		 * @brief Query whether the underlying file handle is open.
		 * @return True when the file is open and ready for writing.
		 */
		bool isOpen() const;

		/**
		 * @brief Close the file and release any resources.
		 */
		void close();

		/**
		 * @brief Write bytes to the file.
		 * @param buffer Pointer to the data to write.
		 * @param size Number of bytes to write from buffer.
		 * @return Number of bytes actually written.
		 */
		size_t write(const void* buffer, size_t size) override;

		/**
		 * @brief Change the current write position in the file.
		 * @param position The absolute position (in bytes) to seek to.
		 * @return The new position in the file after seeking.
		 */
		size_t seek(size_t position) override;

		/**
		 * @brief Get the current write position in the file.
		 * @return Current position in bytes from the start of the file.
		 */
		size_t tell() override;

		/**
		 * @brief Get the total size of the underlying file in bytes.
		 * @return File size in bytes or INVALID_FILE_SIZE on error.
		 */
		size_t getSize() override;

		/**
		 * @brief Constant indicating an invalid file size/position.
		 *
		 * This value is returned by getSize() or tell() when the operation
		 * fails.
		 */
		static inline constexpr size_t INVALID_FILE_SIZE = ~(0);

		/**
		 * @brief Constant indicating an invalid file position.
		 */
		static inline constexpr size_t INVALID_FILE_POSITION = ~(0);

	private:
#ifdef MCE_PLATFORM_WINDOWS
		/**
		 * @brief Native Windows file handle pointer (opaque).
		 *
		 * Stored as void* to avoid including Windows headers in this header
		 * file. The implementation file should treat this as a HANDLE.
		 */
		void* hFile = nullptr;
#elif defined(MCE_PLATFORM_LINUX) || defined(MCE_PLATFORM_MACOS)
		/**
		 * @brief Standard C FILE* used on POSIX platforms.
		 */
		std::FILE* file = nullptr;
#endif
	};
}