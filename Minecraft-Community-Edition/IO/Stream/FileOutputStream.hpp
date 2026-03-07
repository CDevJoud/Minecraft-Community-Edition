#pragma once

#include "OutputStream.hpp"

namespace mce {
	class FileOutputStream final : public OutputStream {
	public:
		void flush() override;
		size_t write(void* buffer, size_t size) override;
		size_t seek(size_t position) override;
		size_t tell() override;
		size_t getSize() override;
	};
}