#include "IRenderer.hpp"

namespace dms::gfx {
	void IRenderer::addFlag(Clear cFlag) {
		this->clearFlags |= cFlag;
	}
	void IRenderer::removeFlag(Clear cFlag) {
		this->clearFlags &= ~cFlag;
	}
	void IRenderer::toggleFlag(Clear cFlag) {
		this->clearFlags ^= cFlag;
	}
	constexpr bool IRenderer::hasFlag(IRenderer::Clear flag) const {
		return (toUnderlying(this->clearFlags) & toUnderlying(flag)) != 0;
	}
}
