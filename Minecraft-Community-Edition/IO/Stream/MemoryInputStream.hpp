#pragma once
#include "InputStream.hpp"

namespace mce {
/**
 * @brief Memory-backed input stream.
 *
 * Provides a read-only InputStream view over an existing memory buffer. The
 * stream does not take ownership of the provided memory; the caller is
 * responsible for ensuring the lifetime of the memory buffer exceeds the
 * lifetime of the MemoryInputStream instance.
 */
	class MemoryInputStream final : public InputStream {
	public:
		/**
		 * @brief Construct a memory input stream over existing data.
		 * @param data Pointer to the memory buffer to read from. Must not be nullptr unless size is zero.
		 * @param size Size of the buffer in bytes.
		 */
		MemoryInputStream(const void* data, size_t size);

		/**
		 * @brief Read bytes from the memory buffer into the provided buffer.
		 * @param buffer Destination buffer to receive data.
		 * @param size Maximum number of bytes to read from the stream.
		 * @return Number of bytes actually read.
		 */
		virtual size_t read(void* buffer, size_t size) override;

		/**
		 * @brief Read a NUL-terminated string from the stream.
		 *
		 * The function does not allocate new memory for the string. Instead it
		 * returns a pointer into the original memory buffer via the buffer
		 * reference parameter. The returned pointer is valid only while the
		 * original memory and this stream remain valid.
		 *
		 * @param buffer Reference to a char* which will be set to point to the string inside the memory buffer.
		 * @param size Reference to a size_t which will be set to the length of the string (excluding the terminating NUL).
		 * @return Number of bytes consumed from the stream, including the terminating NULL byte.
		 */
		size_t readString(char*& buffer, size_t& size);

		/**
		 * @brief Seek to an absolute position in the memory buffer.
		 * @param position Absolute position in bytes from the start of the buffer.
		 * @return The new position after seeking.
		 */
		virtual size_t seek(size_t position) override;

		/**
		 * @brief Get the current read position in the buffer.
		 * @return Current position in bytes from the start of the buffer.
		 */
		virtual size_t tell() override;

		/**
		 * @brief Get the total size of the memory buffer.
		 * @return Size of the buffer in bytes.
		 */
		virtual size_t getSize() override;

	private:
		/**
		 * @brief Pointer to the start of the input memory buffer (not owned).
		 */
		const char* data = nullptr;

		/**
		 * @brief Total size of the input buffer in bytes.
		 */
		size_t size;

		/**
		 * @brief Current read offset from the start of the buffer.
		 */
		size_t offset; // the reading position
	};
}
