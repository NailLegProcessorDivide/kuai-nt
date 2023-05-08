#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "kuai/Events/Event.h"
#include "kuai/Events/AppEvent.h"
#include "Window.h"
#include "Timer.h"

namespace kuai {
	/** \class App
	*   \brief This class runs your game. It handles windowing, events and layer updates.
	*	It uses layers to execute game logic.
	*	@see Layer
	*/
	class App {
	public:
		App();
		virtual ~App();

		/**
		* Starts the application mainloop which in turn updates all layers.
		*/
		void run();

		/**
		* Called every time an event occurs
		*/
		void onEvent(Event* e);

		/**
		* Pushes a layer on to the layer stack.
		*/
		void pushLayer(Layer* layer);

		void addWindow(const WindowProps& props);

		void removeWindow();

		/**
		* Returns the app instance.
		*/
		inline static App& get() { return *instance; }

		/**
		* Returns vector of window instances.
		*/
		std::vector<Box<Window>>& getWindows() { return windows; }
		Window* getActiveWindow();

	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

	private:
		std::vector<Box<Window>> windows;

		Timer timer;
		bool running = true;
		bool minimised = false;
		LayerStack layerStack;

		static App* instance;
	};
}

