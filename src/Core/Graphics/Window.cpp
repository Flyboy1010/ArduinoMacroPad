#include "Core/Graphics/Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// WINDOW CLASS

Window::Window()
{
	m_width = 0;
	m_height = 0;
	m_fullscreen = false;
	m_vsync = true;
	m_nativeWindowPtr = nullptr;
}

Window::Window(const WindowSpecs& specs)
{
	Create(specs);
}

Window::~Window()
{
	glfwDestroyWindow((GLFWwindow*)m_nativeWindowPtr);
	glfwTerminate();
}

bool Window::Create(const WindowSpecs& specs)
{
	// init glfw

	if (!glfwInit())
	{
		std::cout << "GLFW FAILED" << std::endl;
		return false;
	}

	// window hints

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);// the window will stay hidden after creation
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set opengl 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // core profile
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	// create window

	GLFWwindow* window = glfwCreateWindow(specs.width, specs.height, specs.title.c_str(), specs.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	// set window user pointer to be this window

	glfwSetWindowUserPointer(window, this);

	// Make the OpenGL context current

	glfwMakeContextCurrent(window);

	// set vsync

	glfwSwapInterval(specs.vsync);

	// make window visible

	glfwShowWindow(window);

	// init glew

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW FAILED" << std::endl;
		return false;
	}

	// init properties

	m_title = specs.title;
	m_width = specs.width;
	m_height = specs.height;
	m_fullscreen = specs.fullscreen;
	m_vsync = specs.vsync;
	m_nativeWindowPtr = window;

	// success

	return true;
}

void Window::SetTitle(const std::string& title)
{
	m_title = title;

	glfwSetWindowTitle((GLFWwindow*)m_nativeWindowPtr, title.c_str());
}

void Window::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;

	glfwSetWindowSize((GLFWwindow*)m_nativeWindowPtr, width, height);
}

void Window::SetFullscreen(bool fullscreen)
{
	m_fullscreen = fullscreen;

	if (fullscreen)
		glfwSetWindowMonitor((GLFWwindow*)m_nativeWindowPtr, glfwGetPrimaryMonitor(), 0, 0, m_width, m_height, 0);
	else
		glfwSetWindowMonitor((GLFWwindow*)m_nativeWindowPtr, nullptr, 0, 0, m_width, m_height, 0);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers((GLFWwindow*)m_nativeWindowPtr);
}

void Window::SetCloseCallback(const std::function<void(Window&)>& callback)
{
	m_closeCallback = callback;

	glfwSetWindowCloseCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_closeCallback;

		if (callback)
			callback(thisWindow);
	});
}

void Window::SetResizeCallback(const std::function<void(Window&, int, int)>& callback)
{
	m_resizeCallback = callback;

	glfwSetFramebufferSizeCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int width, int height) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		// set the new size of the window

		thisWindow.m_width = width;
		thisWindow.m_height = height;

		// call the callback

		auto& callback = thisWindow.m_resizeCallback;

		if (callback)
			callback(thisWindow, width, height);
	});
}

void Window::SetFiledropCallback(const std::function<void(Window&, int, const char**)>& callback)
{
	m_filedropCallback = callback;

	glfwSetDropCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int count, const char** paths) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_filedropCallback;

		if (callback)
			callback(thisWindow, count, paths);
	});
}

void Window::SetFocusCallback(const std::function<void(Window&, bool)>& callback)
{
	m_focusCallback = callback;

	glfwSetWindowFocusCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int focused) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_focusCallback;

		if (callback)
			callback(thisWindow, focused);
	});
}

void Window::SetKeyCallback(const std::function<void(Window&, int, int, int, int)>& callback)
{
	m_keyCallback = callback;

	glfwSetKeyCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_keyCallback;

		if (callback)
			callback(thisWindow, key, scancode, action, mods);
	});
}

void Window::SetMousePosCallback(const std::function<void(Window&, const glm::vec2&)>& callback)
{
	m_mousePosCallback = callback;

	glfwSetCursorPosCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, double xPos, double yPos) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_mousePosCallback;

		if (callback)
		{
			glm::vec2 mousePos = { (float)xPos, (float)yPos };
			callback(thisWindow, mousePos);
		}
	});
}

void Window::SetMouseButtonCallback(const std::function<void(Window&, int, int, int)>& callback)
{
	m_mouseButtonCallback = callback;

	glfwSetMouseButtonCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, int button, int action, int mods) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_mouseButtonCallback;

		if (callback)
			callback(thisWindow, button, action, mods);
	});
}

void Window::SetMouseScrollCallback(const std::function<void(Window&, float)>& callback)
{
	m_mouseScrollCallback = callback;

	glfwSetScrollCallback((GLFWwindow*)m_nativeWindowPtr, [](GLFWwindow* window, double xoffset, double yoffset) {
		Window& thisWindow = *(Window*)glfwGetWindowUserPointer(window);

		auto& callback = thisWindow.m_mouseScrollCallback;

		if (callback)
			callback(thisWindow, (float)yoffset);
		});
}
