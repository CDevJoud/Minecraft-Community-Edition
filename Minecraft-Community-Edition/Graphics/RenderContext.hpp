#pragma once
#include <EASTL/unique_ptr.h>
#include "Core/QEventBus.hpp"

namespace sf {
	class WindowBase;
}
namespace bgfx {
	struct FrameBufferHandle;
}

namespace mce::gfx {
	class RenderContext {
	public:
		enum class API {
			Noop,       //!< No rendering.
			Agc,        //!< AGC
			Direct3D11, //!< Direct3D 11.0
			Direct3D12, //!< Direct3D 12.0
			Gnm,        //!< GNM
			Metal,      //!< Metal
			Nvn,        //!< NVN
			OpenGLES,   //!< OpenGL ES 2.0+
			OpenGL,     //!< OpenGL 2.1+
			Vulkan,     //!< Vulkan
			WebGPU,     //!< WebGPU

			Count
		};

		RenderContext(core::QEventBus& qBus);
		virtual ~RenderContext() = default;

		virtual bool init(sf::WindowBase& mainWindow, API api) = 0;
		virtual void shutdown() = 0;

		virtual void beginFrame() = 0;
		virtual void endFrame() = 0;

		virtual unsigned short registerWindow(sf::WindowBase& window) = 0;
		virtual void unregisterWindow(uint16_t viewId) = 0;

		virtual void resize(uint16_t viewId, uint32_t width, uint32_t height) = 0;

		virtual bgfx::FrameBufferHandle getFrameBufferHandle(uint16_t viewId) = 0;

		//static eastl::unique_ptr<RenderContext> create(QEventBus& qBus);

		API getRenderAPI() {
			return backendAPI;
		}
	protected:
		core::QEventBus& qBus;
		API backendAPI;
	};
}
