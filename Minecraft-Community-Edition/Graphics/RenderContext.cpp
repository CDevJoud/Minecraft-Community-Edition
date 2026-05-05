#include "RenderContext.hpp"

using mce::core::QEventBus;

namespace mce::gfx {
	RenderContext::RenderContext(QEventBus& qBus) : qBus(qBus) {
	
	}

	/*eastl::unique_ptr<RenderContext> mce::gfx::RenderContext::create(QEventBus& qBus) {
		return eastl::make_unique<RenderContext>(qBus);
	}*/
}