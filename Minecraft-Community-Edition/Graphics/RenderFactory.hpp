#pragma once
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/unordered_map.h>
#include "VertexBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "IndexBuffer.hpp"
#include "FrameBuffer.hpp"
#include "Core/QEventBus.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <format>

namespace mce::gfx {
	// ----------------------------
	// Fast hash (FNV-1a)
	// ----------------------------
	uint64_t hashMemory(const void* data, size_t size);
	// Lol fun fact to keep in mind for future me! Hash collisions are possibile but extremely rare XD

	struct VertexBufferKey {
		uint64_t datahash;
		uint32_t size;
		uint32_t count;
		uint32_t layoutHash;
	};
	struct VertexBufferKeyHash {
		size_t operator()(const VertexBufferKey& k) const;
	};
	struct VertexBufferKeyEqual {
		bool operator()(const VertexBufferKey a, const VertexBufferKey b) const;
	};

	struct ShaderProgramKey {
		uint32_t vsHash;
		uint32_t fsHash;
	};

	struct ShaderProgramKeyHash {
		size_t operator()(const ShaderProgramKey& k) const;
	};

	struct ShaderProgramKeyEqual {
		bool operator()(const ShaderProgramKey a, const ShaderProgramKey b) const;
	};

	struct TextureKey {
		uint64_t datahash;
		uint32_t width;
		uint32_t height;
		uint32_t format;
	};

	struct TextureKeyHash {
		size_t operator()(const TextureKey& k) const;
	};

	struct TextureKeyEqual {
		bool operator()(const TextureKey a, const TextureKey b) const;
	};

	struct IndexBufferKey {
		uint64_t datahash;
		uint32_t size;
	};

	struct IndexBufferKeyHash {
		size_t operator()(const IndexBufferKey& k) const;
	};

	struct IndexBufferKeyEqual {
		bool operator()(const IndexBufferKey a, const IndexBufferKey b) const;
	};
	
	struct FrameBufferKey {
		uint64_t datahash;
		uint32_t width;
		uint32_t height;
	};

	struct FrameBufferKeyHash {
		size_t operator()(const FrameBufferKey& k) const;
	};

	struct FrameBufferKeyEqual {
		bool operator()(const FrameBufferKey a, const FrameBufferKey b) const;
	};

	class RenderFactory {
	public:
		RenderFactory(core::QEventBus& qBus);
		
		
		eastl::shared_ptr<VertexBuffer> createVertexBuffer(const bgfx::Memory* mem, bgfx::VertexLayout vLayout, flags::Buffer bFlag, const std::string& dbgName = "");
		eastl::shared_ptr<VertexBuffer> createVertexBuffer(const VertexArray& vArray, flags::Buffer bFlag, const std::string& dbgName = "");
		eastl::shared_ptr<ShaderProgram> createShaderProgram(const eastl::pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>& shader, bool destroyShaderBin = false, const std::string& dbgName = "");
		eastl::shared_ptr<Texture> createTexture(const bgfx::Memory* bytes);
		eastl::shared_ptr<Texture> createTexture(const eastl::vector<uint8_t>& bytes);
		eastl::shared_ptr<Texture> createTexture(sf::Vector2i size, bool hasMipMap, uint16_t numLayers, bgfx::TextureFormat::Enum textureFormat, uint64_t flags = BGFX_TEXTURE_NONE, const bgfx::Memory* mem = nullptr, uint64_t external = 0);
		eastl::shared_ptr<IndexBuffer> createIndexBuffer(const bgfx::Memory* indices, flags::Buffer bFlag);
		eastl::shared_ptr<FrameBuffer> createFrameBuffer(uint16_t width, uint16_t height, bgfx::TextureFormat::Enum fmt, uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		eastl::shared_ptr<FrameBuffer> createFrameBuffer(uint8_t num, bgfx::Attachment* attachments, bool destroyTexture);
	private:
		std::unordered_map<VertexBufferKey, 
			eastl::weak_ptr<VertexBuffer>, 
			VertexBufferKeyHash, 
			VertexBufferKeyEqual> vbCache;

		std::unordered_map<ShaderProgramKey,
			eastl::weak_ptr<ShaderProgram>,
			ShaderProgramKeyHash,
			ShaderProgramKeyEqual> spCache;

		std::unordered_map<TextureKey,
			eastl::weak_ptr<Texture>,
			TextureKeyHash,
			TextureKeyEqual> texCache;

		std::unordered_map<IndexBufferKey,
			eastl::weak_ptr<IndexBuffer>,
			IndexBufferKeyHash,
			IndexBufferKeyEqual> idxCache;

		std::unordered_map<FrameBufferKey,
			eastl::weak_ptr<FrameBuffer>,
			FrameBufferKeyHash,
			FrameBufferKeyEqual> fbCache;
		mutable std::shared_mutex mutex;
		core::QEventBus& qBus;
	};
}
