#pragma once

#include "kuai/Core/Core.h"
#include "kuai/Events/Event.h"
#include "kuai/Components/EntityComponentSystem.h"

namespace kuai {
	/** \class Layer
	*	\brief A layer describes a self-contained section of game logic or rendering. The App class uses layers in order to run a game.
	*		   The user must create their own layers by inheriting from this class. 
	*		   Any necessary initialisation can be handled in the constructor.
	*		   Code within the update() function will be called every frame.
	*/
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		/**
		* Called when the layer is attached to the app via App::pushLayer().
		*/
		virtual void onAttach() {}
		/**
		* Called when the layer is removed from the app.
		*/
		virtual void onDetach() {}
		/**
		* Called every frame.
		* @param dt Delta time: time elapsed between frames.
		*/
		virtual void update(float dt) {}
		/**
		* Called when an event reaches this layer.
		*/
		virtual void onEvent(Event* event) {}

		inline const std::string& getName() const { return name; }
	protected:
		Box<EntityComponentSystem> ECS;

		std::string name;
	};
}


