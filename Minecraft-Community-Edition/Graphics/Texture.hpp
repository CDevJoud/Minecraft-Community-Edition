#pragma once
#include <EASTL/vector.h>
#include "../libs/bgfx/bgfx.h"
#include <Core/Vector2.hpp>
#include <eastl/shared_ptr.h>

namespace mce::gfx {

	class RenderFactory;

	//Not done needs more stuff to be added
	class Texture {
		friend class RenderFactory;
		template <typename... Args>
		static eastl::shared_ptr<Texture> createInstance(Args&&... args) {
			struct EnableMakeShared : public Texture {
				EnableMakeShared(Args&&... a) : Texture(eastl::forward<Args>(a)...) {}
			};
			return eastl::make_shared<EnableMakeShared>(eastl::forward<Args>(args)...);
		}
		Texture(const bgfx::Memory* tag, bool& success);
		Texture(const eastl::vector<uint8_t>& bytes, bool& success);
		~Texture();
	public:
		
		bgfx::TextureHandle getTextureHandle() const;

		sf::Vector2u getSize() const;
	private:
		sf::Vector2u size;
		bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	};
}
