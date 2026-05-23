#include "VertexBuffer.hpp"

namespace mce::gfx {
	/*VertexBuffer_t::VertexBuffer(const VertexArray& vArray, flags::Buffer bFlags, bool& success) 
	VertexBuffer::~VertexBuffer() 
	bgfx::VertexBufferHandle VertexBuffer::getNativeHandle() const 

	void VertexBuffer::bind(const VertexBuffer& vBuffer, uint8_t stream) {
		bgfx::setVertexBuffer(stream, vBuffer.vbh);
	}
    void VertexBuffer::bind(const eastl::shared_ptr<VertexBuffer>& vBuffer, uint8_t stream) 
	*/
}


/*eastl::shared_ptr<Texture> RenderFactory::createTexture(const bgfx::Memory* bytes) {
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
	}*/