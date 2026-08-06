#pragma once
#include "ShaderProgram.hpp"
#include "Flags.hpp"
#include "Transform.hpp"

namespace dms::gfx {
	struct RenderStates {
		RenderStates() = default;
		~RenderStates() = default;
		eastl::shared_ptr<ShaderProgram> shader;
		eastl::shared_ptr<Texture> texture;
		flags::State states;
		Transform transform;
	};
}
