#include "kpch.h"
#include "App.h"
#include "Log.h"

#include "kuai/Renderer/Renderer.h"
#include "kuai/Sound/AudioManager.h"
#include "Systems.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define BIND(x) std::bind(x, this, std::placeholders::_1)

namespace kuai {

	App* App::instance = nullptr;

	App::App()
	{
		Log::Init();

		KU_CORE_ASSERT(!instance, "Application already exists");
		instance = this;

		addWindow(WindowProps());
		running = true;

		Renderer::init();
		AudioManager::init();

		ECS = new EntityComponentSystem();

		ECS->registerComponent<Transform>();
		ECS->registerComponent<Cam>();
		ECS->registerComponent<Light>();
		ECS->registerComponent<MeshRenderer>();
		ECS->registerComponent<Listener>();
		ECS->registerComponent<SoundSource>();

		renderSys = ECS->registerSystem<RenderSystem>();
		renderSys->acceptSubset(true);
		ECS->setSystemMask<RenderSystem>(BIT(ECS->getComponentType<MeshRenderer>()));

		mainCam = makeBox<Entity>(ECS);
		mainCam->addComponent<Cam>(
			60.0f,
			(float)App::get().getWindow()->getWidth() / (float)App::get().getWindow()->getHeight(),
			0.1f,
			100.0f
		);
		Renderer::setCamera(mainCam->getComponent<Cam>());
	}

	App::~App() 
	{
		delete ECS;
	}

	void App::run() 
	{
		while (running) 
		{
			float elapsedTime = timer.getElapsed(); // Time since last frame
			//KU_CORE_INFO("FPS: {0}", 1.0f / elapsedTime);
						
			if (!minimised)
			{
				update(elapsedTime);
				renderSys->update(elapsedTime);
			}
			for (auto& window : windows)
			{
				window->update();
			}
		}

		AudioManager::cleanup();
		Renderer::cleanup();
	}

	void App::addWindow(const WindowProps& props)
	{
		auto window = Window::create(props);
		window->setEventCallback(BIND(&App::onEventP));
		windows.push_back(std::move(window));
	}

	void App::removeWindow()
	{
	}

	void App::onEventP(Event* e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND(&App::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND(&App::onWindowResize));
		onEvent(e);
	}

	bool App::onWindowClose(WindowCloseEvent& e)
	{
		running = false;
		return true;
	}

	bool App::onWindowResize(WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			minimised = true;
			return false;
		}

		minimised = false;
		Renderer::setViewport(0, 0, e.getWidth(), e.getHeight());

		return false;
	}

	Window* App::getActiveWindow() {
		assert(windows.size() > 0);
		for (auto& window : windows)
		{
			if (window->isActive()) {
				return window.get();
			}
		}
		return windows[0].get();
	}

}