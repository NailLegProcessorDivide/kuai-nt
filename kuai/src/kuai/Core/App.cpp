#include "App.h"
#include "kpch.h"
#include "App.h"
#include "Log.h"

#include "kuai/Renderer/Renderer.h"
#include "kuai/Sound/AudioManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define BIND(x) std::bind(x, this, std::placeholders::_1)

namespace kuai {

	App* App::instance = nullptr;

	App::App() 
	{
		KU_CORE_ASSERT(!instance, "Application already exists");
		instance = this;

		addWindow(WindowProps());
		running = true;

		Renderer::init();
		AudioManager::init();
	}

	App::~App() 
	{

	}

	void App::run() 
	{
		while (running) 
		{
			float elapsedTime = timer.getElapsed(); // Time since last frame
			//KU_CORE_INFO("FPS: {0}", 1.0f / elapsedTime);
						
			if (!minimised)
			{
				{
					KU_PROFILE_SCOPE("LayerStack update");

					for (Layer* layer : layerStack)
						layer->update(elapsedTime);
				}
			}
			for (auto& window : windows)
			{
				window->update();
			}
		}

		AudioManager::cleanup();
		Renderer::cleanup();
	}

	void App::onEvent(Event* e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND(&App::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND(&App::onWindowResize));

		for (auto it = layerStack.end(); it != layerStack.begin(); ) // Start from end and go backwards through stack
		{
			(*--it)->onEvent(e);
			if (e->handled) // As soon as event handled, break
				break;
		}
	}

	void App::pushLayer(Layer* layer)
	{
		layerStack.pushLayer(layer);
	}

	void App::addWindow(const WindowProps& props)
	{
		auto window = Window::create(props);
		window->setEventCallback(BIND(&App::onEvent));
		windows.push_back(std::move(window));
	}

	void App::removeWindow()
	{
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