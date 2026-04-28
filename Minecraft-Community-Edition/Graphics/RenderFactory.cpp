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

		return h;
	}

	bool TextureKeyEqual::operator()(const TextureKey a, const TextureKey b) const {
		return a.datahash == b.datahash;
	}

	RenderFactory::RenderFactory(QEventBus& qBus) : qBus(qBus) {}

	eastl::shared_ptr<VertexBuffer> mce::gfx::RenderFactory::createVertexBuffer(const VertexArray& vArray, flags::Buffer bFlag, const std::string& dbgName) {
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
}