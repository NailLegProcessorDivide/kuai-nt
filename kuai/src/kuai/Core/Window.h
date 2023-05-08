#pragma once

#include "kpch.h"

#include "kuai/Core/Core.h"
#include "kuai/Events/Event.h"

namespace kuai {

	/**
	* Properties of a window, i.e., title, width and height.
	*/
	struct WindowProps
	{
		std::string title;
		uint32_t width;
		uint32_t height;
		uint32_t screenX;
		uint32_t screenY;

		WindowProps(const std::string& title = "Demo",
			uint32_t width = 1024,
			uint32_t height = 768,
			uint32_t screenX = 200,
			uint32_t screenY = 200) :
			title(title),
			width(width),
			height(height),
			screenX(screenX),
			screenY(screenY) {}
	};

	/**
	* Interface for representing a desktop window
	*/
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event*)>;

		virtual ~Window() {}

		virtual void update() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual void setSize(uint32_t, uint32_t) = 0;

		virtual uint32_t getXPos() const = 0;
		virtual uint32_t getYPos() const = 0;
		virtual void setPos(uint32_t, uint32_t) = 0;


		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;
		virtual bool isActive() const = 0;

		static Box<Window> create(const WindowProps& props = WindowProps());
	};
}