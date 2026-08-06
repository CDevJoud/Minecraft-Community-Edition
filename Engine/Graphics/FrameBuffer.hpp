#pragma once
#include <bgfx/bgfx.h>
#include <EASTL/shared_ptr.h>

namespace dms::gfx {
	class RenderFactory;
	
	class FrameBuffer {
		friend class RenderFactory;
		
		template<typename... Args>
		static eastl::shared_ptr<FrameBuffer> createInstance(Args&&... args) {
			struct EnableMakeShared : public FrameBuffer {
				EnableMakeShared(Args&&... a) : FrameBuffer(eastl::forward<Args>(a)...) {}
			};
			return eastl::make_shared<EnableMakeShared>(eastl::forward<Args>(args)...);
		}
		FrameBuffer(uint16_t width, uint16_t height, bgfx::TextureFormat::Enum fmt,bool& success, uint64_t flag = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		FrameBuffer(uint8_t num, bgfx::Attachment* attachments, bool destroyTexture, bool& success);
		~FrameBuffer();
	
	public:
		bgfx::FrameBufferHandle getNativeHandle() const;

	private:
		bgfx::FrameBufferHandle fbh = BGFX_INVALID_HANDLE;
		uint16_t width;
		uint16_t height;

	};
}