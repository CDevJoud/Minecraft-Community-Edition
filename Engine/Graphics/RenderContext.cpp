#include "RenderContext.hpp"

using dms::core::QEventBus;

namespace dms::gfx {
	RenderContext::RenderContext(QEventBus& qBus) : qBus(qBus) {
	
	}

	/*eastl::unique_ptr<RenderContext> dms::gfx::RenderContext::create(QEventBus& qBus) {
		return eastl::make_unique<RenderContext>(qBus);
	}*/
}