#include "FrameBuffer.hpp"

namespace dms::gfx{
	FrameBuffer::FrameBuffer(uint16_t width, uint16_t height, bgfx::TextureFormat::Enum fmt, bool& success, uint64_t flag) {
		if (!bgfx::isValid(FrameBuffer::fbh)) {
			FrameBuffer::fbh = bgfx::createFrameBuffer(width, height, fmt, flag);
		}
		success = bgfx::isValid(FrameBuffer::fbh);
		this->width = width;
		this->height = height;
	}
	FrameBuffer::FrameBuffer(uint8_t num, bgfx::Attachment* attachments, bool destroyTexture, bool& success) {
		if (!bgfx::isValid(FrameBuffer::fbh)) {
			FrameBuffer::fbh = bgfx::createFrameBuffer(num, attachments, destroyTexture);
		}
		success = bgfx::isValid(FrameBuffer::fbh);
		this->width = 0;
		this->height = 0;
	}
	FrameBuffer::~FrameBuffer() {
		if (bgfx::isValid(FrameBuffer::fbh)) {
			bgfx::destroy(fbh);
			FrameBuffer::fbh = BGFX_INVALID_HANDLE;
		}
	}
	bgfx::FrameBufferHandle FrameBuffer::getNativeHandle() const {
		return FrameBuffer::fbh;
	}
}

