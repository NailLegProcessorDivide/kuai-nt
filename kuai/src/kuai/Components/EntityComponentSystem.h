#pragma once

#include <memory>
#include <list>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

#include "kuai/Events/Event.h"

namespace kuai {

	class IFnHandler
	{
	public:
		void exec(Event& event) { call(event); }
	private:
		virtual void call(Event& event) = 0;
	};
	
	// Class to hold reference to member function and offset of method to be called
	template<class T, class EventType>
	class FnHandler : public IFnHandler
	{
	public:
		typedef void (T::* MemberFn)(EventType&);

		FnHandler(T* instance, MemberFn memberFn) : instance{ instance }, memberFn{ memberFn } {}

		void call(Event& event)
		{
			// Cast event to the correct type and call member function
			(instance->*memberFn)(*(EventType*)(&event));
		}
	private:
		T* instance; // Pointer to instance class

		MemberFn memberFn;
	};

	using HandlerList = std::list<IFnHandler*>;

	class EventBus
	{
	public:
		template<typename EventType>
		void notify(EventType& event)
		{
			HandlerList* handlers = subscribers[typeid(EventType).name()];
			
			if (!handlers)
				return;

			for (auto& handler : *handlers) 
			{
				if (handler != nullptr) 
				{
					handler->exec(event);
				}
			}
		}

		template<typename T, typename EventType>
		void subscribe(T* instance, void (T::*memberFn)(EventType&))
		{
			HandlerList* handlers = subscribers[typeid(EventType).name()];

			if (!handlers) // First time init
			{
				handlers = new HandlerList();
				subscribers[typeid(EventType).name()] = handlers;
			}
			
			handlers->push_back(new FnHandler<T, EventType>(instance, memberFn));
		}
	private:
		std::unordered_map<const char*, HandlerList*> subscribers;
	};

	class EntityComponentSystem
	{
	public:
		EntityComponentSystem()
		{
			entityManager = makeBox<EntityManager>();
			componentManager = makeBox<ComponentManager>();
			systemManager = makeBox<SystemManager>();
			eventBus = makeBox<EventBus>();
		}

		// *** Entity Management **********************************************

		EntityID createEntity()
		{
			return entityManager->createEntity();
		}

		void destroyEntity(EntityID id)
		{
			systemManager->onEntityDestroyed(id);
			entityManager->destroyEntity(id);
			componentManager->onEntityDestroyed(id);
		}

		// *** Component Management *******************************************

		template<typename T>
		void registerComponent()
		{
			componentManager->registerComponent<T>();
		}

		template<typename T, typename ...Args>
		void addComponent(EntityID id, Args&& ...args)
		{
			componentManager->addComponent<T>(id, std::forward<Args>(args)...);

			auto componentMask = entityManager->getComponentMask(id);
			componentMask |= BIT(componentManager->getComponentType<T>());

			entityManager->setComponentMask(id, componentMask);
			systemManager->onEntityComponentMaskChanged(id, componentMask);
		}

		template<typename T>
		void removeComponent(EntityID id)
		{
			componentManager->removeComponent<T>(id);

			auto componentMask = entityManager->getComponentMask(id);
			componentMask &= BIT(componentManager->getComponentType<T>()) ^ std::numeric_limits<ComponentMask>::max();

			entityManager->setComponentMask(id, componentMask);
			systemManager->onEntityComponentMaskChanged(id, componentMask);
		}

		template<typename T>
		T& getComponent(EntityID id)
		{
			return componentManager->getComponent<T>(id);
		}

		template<typename T>
		bool hasComponent(EntityID id)
		{
			return componentManager->hasComponent<T>(id);
		}

		template<typename T>
		ComponentType getComponentType()
		{
			return componentManager->getComponentType<T>();
		}


		// *** System Management **********************************************

		template<typename T>
		Rc<T> registerSystem()
		{
			auto sm = systemManager->registerSystem<T>();
			sm->ECS = this;
			sm->init();
			return sm;
		}

		template<typename T>
		void setSystemMask(ComponentMask mask)
		{
			systemManager->setComponentMask<T>(mask);
		}

		// *** Event Management (of systems) **********************************

		template<typename EventType>
		void notifySystems(EventType& event)
		{
			eventBus->notify(event);
		}

		template<typename T, typename EventType>
		void subscribeSystem(T* instance, void (T::* memberFn)(EventType&))
		{
			eventBus->subscribe(instance, memberFn);
		}

	private:
		Box<EntityManager> entityManager;
		Box<ComponentManager> componentManager;
		Box<SystemManager> systemManager;

		// *** Event bus - for communication between systems ***
		Box<EventBus> eventBus;
	};
}