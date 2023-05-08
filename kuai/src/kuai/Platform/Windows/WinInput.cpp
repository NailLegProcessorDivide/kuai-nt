#include "kpch.h"
#include "kuai/Core/Input.h"

#include "kuai/Core/App.h"
#include <GLFW/glfw3.h>

namespace kuai {

	bool Input::isKeyPressed(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(App::get().getActiveWindow()->getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseBtnPressed(MouseBtnCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(App::get().getActiveWindow()->getNativeWindow());
		auto state = glfwGetMouseButton(window, keycode);

		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePos()
	{
		auto window = static_cast<GLFWwindow*>(App::get().getActiveWindow()->getNativeWindow());
		double xpos, ypos;
		int windx, windy;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwGetWindowPos(window, &windx, &windy);

		return glm::vec2((float)(xpos + windx), (float)(ypos + windy));
	}

	float Input::getMouseX()
	{
		return Input::getMousePos().x;
	}

	float Input::getMouseY()
	{
		return Input::getMousePos().y;
	}
}


