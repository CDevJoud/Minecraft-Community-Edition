#include "Minecraft.hpp"
#include "IO/Logger.hpp"
#include "IO/LoggerSinks.hpp"
#include <filesystem>

#include "Graphics/BgfxRenderContext.hpp"
#include <Core/Sleep.hpp>
#include <Core/Event.hpp>
#include <optional>
#include <Graphics/RenderFactory.hpp>

#ifdef MCE_PLATFORM_WINDOWS
#pragma warning(disable:4996)
#endif

#define LOG_INFO(msg) qBus.post(event::Log(event::Log::INFO, msg))
#define LOG_ERROR(msg) qBus.post(event::Log(event::Log::ERROR, msg))

namespace mce {
	using core::QEventBus;
	using gfx::RenderContext;
	using gfx::RenderFactory;
	using io::VirtualFileSystem;

	Minecraft::Minecraft(const eastl::string_view& profileName, QEventBus& qBus, uint16_t viewId, sf::WindowHandle window, sf::Vector2u viewSize, eastl::shared_ptr<RenderContext>& renderCtx, RenderFactory& factory, VirtualFileSystem& vfs) :
		qBus(qBus),
		profileName(profileName.data(), profileName.size()),
		window(window),
		bIsRunning(false),
		renderCtx(renderCtx),
		viewSize(viewSize),
		viewId(viewId),
		renderer(viewId, factory, renderCtx->getRenderAPI()),
		vfs(vfs) {
		gfx::IRenderer* interface = (gfx::IRenderer*)&renderer;
		interface->setViewSpace(0, 0, viewSize.x, viewSize.y);
		this->onClose = this->qBus.subscribeRAII<event::window::Close>([this](const event::window::Close& e) {
			if (e.window == this->window) {
				Minecraft::bIsRunning = false;
			}
			});

		this->onResize = this->qBus.subscribeRAII<event::window::Resize>([this](const event::window::Resize& e) {
			if (e.window == this->window) {
				this->viewSize.x = e.newSize.x;
				this->viewSize.y = e.newSize.y;
				this->renderCtx->resize(this->viewId, this->viewSize.x, this->viewSize.y);
				gfx::IRenderer* interface = (gfx::IRenderer*)&renderer;
				interface->setViewSpace(0, 0, this->viewSize.x, this->viewSize.y);
			}
			});

		gfx::VertexArray vArray;
		vArray.append(gfx::Vertex(sf::Vector3f(-0.5f, -0.5f, 0.0f), gfx::Color::Black, sf::Vector2f(0.0f, 1.0f)));
		vArray.append(gfx::Vertex(sf::Vector3f( 0.5f, -0.5f, 0.0f), gfx::Color::Black, sf::Vector2f(1.0f, 1.0f)));
		vArray.append(gfx::Vertex(sf::Vector3f( 0.0f,  0.5f, 0.0f), gfx::Color::Black, sf::Vector2f(0.5f, 0.0f)));
		vLayout = gfx::Vertex::layout();
		vArray.setVertexLayout(vLayout, sizeof(gfx::Vertex));

		gfx::flags::Buffer vbFlag;
		vbFlag.addFlag(gfx::flags::Buffer::None);

		Minecraft::vb = factory.createVertexBuffer(vArray, vbFlag, "Minecraft:VertexBuffer");
		if (Minecraft::vb == nullptr) {
			qBus.post(event::Log(event::Log::ERROR, "Couldn't create a vertex buffer!"));
		}

		eastl::vector<uint8_t> mem;
		vfs.getFile("assets.images.who", mem);

		renderState.texture = factory.createTexture(mem);

		cube.create(factory, {}, gfx::Color());
		
		cube.setTexture(renderState.texture);
		
		torus.create(qBus, factory, 10, 5, 128, 128, gfx::Color::Black);
		//torus.createPlane(qBus, factory, 100, 100, 20, 20, gfx::Color::Black);
		torus.setTexture(renderState.texture);
	}

	Minecraft::~Minecraft() {

		//renderCtx->shutdown();

		//MCE_INFO("Shutting down");
		//this->onClose;
		
	}
	
	int Minecraft::initInstance() {
		LOG_INFO(std::format("Init MCE: {}", profileName));

		

		renderer.init(vfs);

		/*
		* in the future here would we load the game assets from the vfs
		*/

		Minecraft::bIsRunning = true;

		LOG_INFO("Done");
		return 0;
	}

	int Minecraft::run() {

		int retCode = Minecraft::initInstance();

		if (retCode < 0) {
			return retCode;
		}

		while (Minecraft::bIsRunning) {

			//there is nothing to do so we sleep so we wont let the thread loop so quickly
			sf::sleep(sf::milliseconds(10));
		}
		return 0;
	}

	void Minecraft::render() {
		renderState.transform.position.z = -2.0f;
		renderState.transform.rotation.x += 0.05f;
		//renderer.render(Minecraft::vb, renderState);
		cube.position.z = 20.0f;
		cube.rotation.x += 0.00005f;
		cube.rotation.y += 0.00015f;
		cube.rotation.z += 0.00010f;
		cube.scale = { 3.0f, 3.0f, 3.0f };

		torus.position.z = 20.0f;
		//torus.rotation.x = 310 * (3.14 / 180);
		torus.rotation.y += 0.25 * (3.14 / 180);
		torus.rotation.x += 0.25 * (3.14 / 180);
		//torus.rotation.z += 0.0035f;
		//renderer.render(cube);
		renderer.render(torus);
	}
}
//-config ../data/launcher/launcher.xml --renderer d3d11