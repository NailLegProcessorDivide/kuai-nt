#pragma once

#include "kpch.h"

#include "kuai/Core/Core.h"

namespace kuai {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMove,
		KeyPress, KeyRelease,
		MouseBtnPress, MouseBtnRelease, MouseMove, MouseScroll
	};

	enum EventCategory
	{
		NoneEventCategory = 0,
		AppEventCategory = 1,
		InputEventCategory = 2,
		KeyboardEventCategory = 4,
		MouseEventCategory = 8,
		MouseBtnEventCategory = 16
	};

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	/**
	* Base class for events.
	*/
	class Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }

		bool handled = false;
	};

	class EventDispatcher 
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event* event) : event(event) {}

		template<typename T>
		bool dispatch(EventFn<T> func)
		{
			if (event->getEventType() == T::getStaticType()) // Check if passed event type matches event type of template argument
			{ 
				event->handled |= func(*(T*)event); // Call the function with that event as its argument
				return true; // Indicate the event has been handled
			}
			return false;
		}

	private:
		Event* event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}


