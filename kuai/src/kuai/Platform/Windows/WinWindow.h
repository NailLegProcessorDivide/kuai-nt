#pragma once

#include "kuai/Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// @cond
namespace kuai {
	class WinWindow : public Window
	{
	public:
		WinWindow(const WindowProps& props);
		virtual ~WinWindow();

		void update() override;

		inline uint32_t getWidth() const override { return data.width; }
		inline uint32_t getHeight() const override { return data.height; }
		virtual void setSize(uint32_t, uint32_t) override;

		inline uint32_t getXPos() const override { return data.screenX; }
		inline uint32_t getYPos() const override { return data.screenY; }
		virtual void setPos(uint32_t, uint32_t) override;

		inline void setEventCallback(const EventCallbackFn& callback) override
		{
			data.eventCallback = callback;
		}
		virtual void setVSync(bool enabled) override;
		virtual bool isVSync() const override;

		virtual void* getNativeWindow() const { return window; }
		virtual bool isActive() const override;
	private:
		virtual void init(const WindowProps& props);
		virtual void cleanup();

		GLFWwindow* window;

		struct WindowData
		{
			std::string title;
			uint32_t width, height;
			uint32_t screenX, screenY;
			bool vSync;
			bool isActive;

			EventCallbackFn eventCallback;
		};

		WindowData data;
	};
}
// @endcond