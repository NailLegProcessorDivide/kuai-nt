#pragma once

#include "Core.h"

#include "kuai/Events/Event.h"
#include "kuai/Events/AppEvent.h"
#include "Window.h"
#include "Timer.h"

#include "kuai/Components/EntityComponentSystem.h"
#include "kuai/Components/Entity.h"

namespace kuai {
	/** \class App
	*   \brief This class runs your game. It handles windowing, events and updates.
	*	@see Layer
	*/
	class App {
	public:
		App();
		virtual ~App();

		/**
		* Starts the application mainloop.
		*/
		void run();

		/**
		* Called once per frame.
		*/
		virtual void update(float dt) = 0;

		/**
		* Called every time an event occurs.
		*/
		virtual void onEvent(Event* e) {}

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
		Window* getWindow() { return windows[0].get(); }

		Entity createEntity() { return Entity(ECS); }

		std::optional<Entity> getEntityById(EntityID id)
		{ 
			if (ECS->hasComponent<Transform>(id)) // Every entity has a transform
			{
				return Entity(ECS, id);
			}
			return {};
		}

		//Entity getEntityByName(std::string& name) {}
		
		void destroyEntity(Entity entity) 
		{ 
			if (ECS->hasComponent<Transform>(entity.getId()))
			{
				ECS->destroyEntity(entity.getId());
			}
		}

		Entity* getMainCam() { return mainCam.get(); }

		void setMainCam(Cam& cam)
		{
			mainCam->getComponent<Cam>().isMain = false;
			mainCam->getComponent<Cam>() = cam;
			mainCam->getComponent<Cam>().isMain = true;
		}

	private:
		/**
		* Called every time an event occurs; private callback for parent class only.
		*/
		void onEventP(Event* e);

		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

	private:
		std::vector<Box<Window>> windows;

		Timer timer;
		bool running = true;
		bool minimised = false;

		EntityComponentSystem* ECS;

		Rc<System> renderSys;

		Rc<Entity> mainCam;

		static App* instance;
	};
}

