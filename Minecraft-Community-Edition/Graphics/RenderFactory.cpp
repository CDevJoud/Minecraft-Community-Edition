#include "RenderFactory.hpp"
#include "IO/Logger.hpp"

using mce::core::QEventBus;

#define LOG_INFO(msg) qBus.post(event::Log(event::Log::INFO, msg, "bgfx"))
#define LOG_ERROR(msg) qBus.post(event::Log(event::Log::ERROR, msg, "bgfx"))
namespace mce::gfx {

	uint64_t hashMemory(const void* data, size_t size) {
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
		uint64_t hash = 1469598103934665603ULL;

		for (size_t i = 0; i < size; ++i) {
			hash ^= bytes[i];
			hash *= 1099511628211ULL;
		}

		return hash;
	}

	size_t VertexBufferKeyHash::operator()(const VertexBufferKey& k) const {
		size_t h = k.datahash;
		h ^= k.size + 0x9E3779B9 + (h << 6) + (h >> 2);
		h ^= k.layoutHash + 0x9E3779B9 + (h << 6) + (h >> 2);
		return h;
	}

	bool VertexBufferKeyEqual::operator()(const VertexBufferKey a, const VertexBufferKey b) const {
		return a.datahash   == b.datahash   &&
			   a.size		== b.size       &&
			   a.layoutHash == b.layoutHash;
	}

	size_t ShaderProgramKeyHash::operator()(const ShaderProgramKey& k) const {
		size_t h = 1469598103934665603ULL;
		h ^= k.vsHash + 0x9E3779B9 + (h << 6) + (h >> 2);
		h ^= k.fsHash + 0x9E3779B9 + (h << 6) + (h >> 2);

		return h;
	}

	bool ShaderProgramKeyEqual::operator()(const ShaderProgramKey a, const ShaderProgramKey b) const {
		return a.vsHash == b.vsHash && a.fsHash == b.fsHash;
	}

	size_t TextureKeyHash::operator()(const TextureKey& k) const {
		size_t h = k.datahash;
		h ^= k.width  + 0x9E3379A0 + (h << 6) + (h >> 2);
		h ^= k.height + 0x9E3379B9 + (h << 6) + (h >> 2);
		h ^= k.format + 0x9E3310F1 + (h << 6) + (h >> 2);
		return h;
	}

	bool TextureKeyEqual::operator()(const TextureKey a, const TextureKey b) const {
		return a.datahash == b.datahash;
	}

	size_t IndexBufferKeyHash::operator()(const IndexBufferKey& k) const {
		size_t h = k.datahash;
		h ^= k.size + 0x9E3779B9 + (h << 6) + (h >> 2);

		return h;
	}

	bool IndexBufferKeyEqual::operator()(const IndexBufferKey a, const IndexBufferKey b) const {
		return a.datahash == b.datahash && a.size == b.size;
	}

	size_t FrameBufferKeyHash::operator()(const FrameBufferKey& k) const {
		size_t h = k.datahash;
		h ^= k.width + 0x9E3779B9 + (h << 6) + (h >> 2);
		h ^= k.height + 0x9E3779B9 + (h << 6) + (h >> 2);

		return h;
	}

	bool FrameBufferKeyEqual::operator()(const FrameBufferKey a, const FrameBufferKey b) const {
		return a.datahash == b.datahash && a.width == b.width && a.height == b.height;
	}

	RenderFactory::RenderFactory(QEventBus& qBus) : qBus(qBus) {}

	eastl::shared_ptr<VertexBuffer> RenderFactory::createVertexBuffer(const bgfx::Memory* mem, bgfx::VertexLayout vLayout, flags::Buffer bFlag, const std::string& dbgName) {
		VertexBufferKey key{
			hashMemory(mem->data, mem->size),
			0x9E3779B9,
			0x9E3779B9,
			vLayout.m_hash
		};

		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::vbCache.find(key);
			if (it != vbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		// Note VertexBuffer Creation should not be in the lock!! it kills performance 
		// thats why the LCE perform bad ;)

		//Create new buffer
		LOG_INFO(std::format("Creating a new buffer named {}", dbgName));
		bool isSuccess = false;
		eastl::shared_ptr<VertexBuffer> vb = VertexBuffer::createInstance(mem, vLayout, bFlag, isSuccess);
		bgfx::setName(vb.get()->getNativeHandle(), dbgName.c_str());
		if (!isSuccess) {
			//Error
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::vbCache.find(key);
			if (it != vbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::vbCache[key] = vb;
			return vb;
		}


		RenderFactory::vbCache[key] = vb;
		return vb;
	}

	eastl::shared_ptr<VertexBuffer> RenderFactory::createVertexBuffer(const VertexArray& vArray, flags::Buffer bFlag, const std::string& dbgName) {
		VertexBufferKey key{ 
			hashMemory(vArray.vertices.data(), vArray.vertices.size()),
			vArray.getVertexCount() * vArray.vertexSizeInMem,
			vArray.getVertexCount(),
			vArray.vLayout.m_hash
		};

		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::vbCache.find(key);
			if (it != vbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		// Note VertexBuffer Creation should not be in the lock!! it kills performance 
		// thats why the LCE perform bad ;)

		//Create new buffer
		LOG_INFO(std::format("Creating a new buffer named {}", dbgName));
		bool isSuccess = false;
		eastl::shared_ptr<VertexBuffer> vb = VertexBuffer::createInstance(vArray, bFlag, isSuccess);
		bgfx::setName(vb.get()->getNativeHandle(), dbgName.c_str());
		if (!isSuccess) {
			//Error
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::vbCache.find(key);
			if (it != vbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::vbCache[key] = vb;
			return vb;
		}

		
		RenderFactory::vbCache[key] = vb;
		return vb;
	}

	eastl::shared_ptr<ShaderProgram> RenderFactory::createShaderProgram(const eastl::pair<eastl::vector<uint8_t>, eastl::vector<uint8_t>>& shader, bool destroyShaderBin, const std::string& dbgName) {
		ShaderProgramKey key{
			hashMemory(shader.first.data(), shader.first.size()),
			hashMemory(shader.second.data(), shader.second.size())
		};
		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::spCache.find(key);
			if (it != spCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<ShaderProgram> sp = ShaderProgram::createInstance(shader.first, shader.second, isSuccess, destroyShaderBin, dbgName);
		
		if (!isSuccess) {

			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::spCache.find(key);
			if (it != spCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::spCache[key] = sp;
			return sp;
		}

		RenderFactory::spCache[key] = sp;
		return sp;
	}

	eastl::shared_ptr<Texture> RenderFactory::createTexture(const bgfx::Memory* bytes) {
		TextureKey key{
			hashMemory(bytes->data, bytes->size),
			0,
			0,
			0
		};
		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<Texture> tex = Texture::createInstance(bytes, isSuccess);
		if (!isSuccess) {

			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::texCache[key] = tex;
			return tex;
		}

		RenderFactory::texCache[key] = tex;
		return tex;
	}
	eastl::shared_ptr<Texture> RenderFactory::createTexture(const eastl::vector<uint8_t>& bytes) {
		TextureKey key{
			hashMemory(bytes.data(), bytes.size()),
			0,
			0,
			0
		};
		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<Texture> tex = Texture::createInstance(bytes, isSuccess);
		if (!isSuccess) {

			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::texCache[key] = tex;
			return tex;
		}

		RenderFactory::texCache[key] = tex;
		return tex;
	}

	eastl::shared_ptr<Texture> RenderFactory::createTexture(sf::Vector2i size, bool hasMipMap, uint16_t numLayers, bgfx::TextureFormat::Enum textureFormat, uint64_t flags, const bgfx::Memory* mem, uint64_t external) {
		TextureKey key;
		if (mem == nullptr) {
			key.datahash = hashMemory(&size, sizeof(size));
			key.format = static_cast<uint32_t>(textureFormat);
			key.width = size.x;
			key.height = size.y;
		}
		else {
			key.datahash = hashMemory(mem->data, mem->size);
			key.format = static_cast<uint32_t>(textureFormat);
			key.width = size.x;
			key.height = size.y;
		}
		{
			std::shared_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<Texture> tex = Texture::createInstance(size, hasMipMap, numLayers, textureFormat, isSuccess, flags, mem, external);
		if (!isSuccess) {
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::texCache.find(key);
			if (it != texCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::texCache[key] = tex;
			return tex;
		}

		RenderFactory::texCache[key] = tex;
		return tex;
	}

	eastl::shared_ptr<IndexBuffer> RenderFactory::createIndexBuffer(const bgfx::Memory* indices, flags::Buffer bFlag) {
		IndexBufferKey key{
			hashMemory(indices->data, indices->size),
			indices->size
		};

		{
			std::shared_lock lock(RenderFactory::mutex);
			auto it = RenderFactory::idxCache.find(key);
			if (it != idxCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It Exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<IndexBuffer> idx = IndexBuffer::createInstance(indices, bFlag, isSuccess);
		if (!isSuccess) {
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);
			auto it = RenderFactory::idxCache.find(key);
			if (it != idxCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::idxCache[key] = idx;
			return idx;
		}

		RenderFactory::idxCache[key] = idx;
		return idx;
	}
	eastl::shared_ptr<FrameBuffer> RenderFactory::createFrameBuffer(uint16_t width, uint16_t height, bgfx::TextureFormat::Enum fmt, uint64_t flags) {
		FrameBufferKey key{
			1469598103934665603ULL,
			width,
			height,
		};

		{
			std::shared_lock lock(RenderFactory::mutex);
			auto it = RenderFactory::fbCache.find(key);
			if (it != fbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<FrameBuffer> fb = FrameBuffer::createInstance(width, height, fmt, isSuccess, flags);
		if (!isSuccess) {
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::fbCache.find(key);
			if (it != fbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::fbCache[key] = fb;
			return fb;
		}

		RenderFactory::fbCache[key] = fb;
		return fb;
	}
	eastl::shared_ptr<FrameBuffer> RenderFactory::createFrameBuffer(uint8_t num, bgfx::Attachment* attachment, bool destroyTexture) {
		FrameBufferKey key{
			1469598103934665603ULL,
			static_cast<uint32_t>(num),
			static_cast<uint32_t>(attachment[0].handle.idx)
		};

		{
			std::shared_lock lock(RenderFactory::mutex);
			auto it = RenderFactory::fbCache.find(key);
			if (it != fbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("It exist");
					return existing;
				}
			}
		}

		bool isSuccess = false;
		eastl::shared_ptr<FrameBuffer> fb = FrameBuffer::createInstance(num, attachment, destroyTexture, isSuccess);
		if (!isSuccess) {
			return nullptr;
		}

		{
			std::unique_lock lock(RenderFactory::mutex);

			auto it = RenderFactory::fbCache.find(key);
			if (it != fbCache.end()) {
				if (auto existing = it->second.lock()) {
					LOG_INFO("Another Thread reached here returning existing!");
					return existing; // WTF???? another thread reached here????
				}
			}
			RenderFactory::fbCache[key] = fb;
			return fb;
		}

		RenderFactory::fbCache[key] = fb;
		return fb;
	}
	
}