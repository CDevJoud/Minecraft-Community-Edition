#pragma once
#include <Core/Vector3.hpp>

namespace mce::gfx {
	struct Transform {
		sf::Vector3f position;
		sf::Vector3f scale = { 1.0f, 1.0f, 1.0f };
		sf::Vector3f rotation;
	};
}
