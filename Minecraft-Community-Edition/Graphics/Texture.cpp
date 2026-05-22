#include "Texture.hpp"
#include <Graphics/Image.hpp>

namespace mce::gfx {
	Texture::Texture(const bgfx::Memory* tag, bool& success) {
		gfx::Image img;

		if (img.loadFromMemory(tag->data, tag->size)) {
			const bgfx::Memory* mem = bgfx::copy(img.getPixelsPtr(), img.getSize().x * img.getSize().y * 4);
			Texture::size = img.getSize();
			Texture::texture = bgfx::createTexture2D(size.x, size.y, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE, mem);
			success = bgfx::isValid(Texture::texture);
		}
		else {
			success = false;
		}

	}
	Texture::Texture(sf::Vector2i size, bool hasMipMap, uint16_t numLayers, bgfx::TextureFormat::Enum textureFormat, bool& success, uint64_t flags, const bgfx::Memory* mem, uint64_t external) {
		Texture::texture = bgfx::createTexture2D(size.x, size.y, hasMipMap, numLayers, textureFormat, flags, mem, external);
		success = bgfx::isValid(Texture::texture);
		Texture::size.x = size.x;
		Texture::size.y = size.y;
	}
	Texture::Texture(const eastl::vector<uint8_t>& bytes, bool& success) {
		gfx::Image img;
		
		if (img.loadFromMemory(bytes.data(), bytes.size())) {
			const bgfx::Memory* mem = bgfx::copy(img.getPixelsPtr(), img.getSize().x * img.getSize().y * 4);
			Texture::size = img.getSize();
			Texture::texture = bgfx::createTexture2D(size.x, size.y, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE, mem);
			success = bgfx::isValid(Texture::texture);
		}
		else {
			success = false;
		}
	}
	Texture::~Texture() {
		if (bgfx::isValid(Texture::texture)) {
			bgfx::destroy(Texture::texture);
			Texture::texture = BGFX_INVALID_HANDLE;
		}
	}
    Texture::Texture(const uint16_t idx) : texture(idx) {
		
	}
	Texture& Texture::operator=(const uint16_t idx) {
		this->texture.idx = idx;
		return *this;
	}
	bgfx::TextureHandle Texture::getTextureHandle() const {
		return Texture::texture;
	}
}
