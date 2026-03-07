#pragma once

#include "InputStream.hpp"

namespace mce {
	class FileInputStream final : public InputStream {
	public:
		size_t read(void* buffer, size_t size) override;
		size_t seek(size_t position) override;
		size_t tell() override;
		size_t getSize() override;
	};
}