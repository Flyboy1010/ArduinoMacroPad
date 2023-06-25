#pragma once

#include <string>
#include <functional>
#include <glm/glm.hpp>

struct WindowSpecs
{
	int width, height;
	std::string title;
	bool fullscreen, vsync;
};

class Window
{
public:
	Window();
	Window(const Window&) = delete; // delete copy ctor
	Window(const WindowSpecs& specs);
	~Window();

	bool Create(const WindowSpecs& specs);

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	glm::vec2 GetSize() const { return glm::vec2((float)m_width, (float)m_height); }
	const std::string& GetTitle() const { return m_title; }
	void* GetNativeWindowPtr() const { return m_nativeWindowPtr; }

	void SetTitle(const std::string& title);
	void SetSize(int width, int height);
	void SetFullscreen(bool fullscreen);

	void SwapBuffers();

	// set callbacks

	void SetCloseCallback(const std::function<void(Window&)>& callback);
	void SetResizeCallback(const std::function<void(Window&, int, int)>& callback);
	void SetFiledropCallback(const std::function<void(Window&, int, const char**)>& callback);
	void SetFocusCallback(const std::function<void(Window&, bool)>& callback);
	void SetKeyCallback(const std::function<void(Window&, int, int, int, int)>& callback);
	void SetMousePosCallback(const std::function<void(Window&, const glm::vec2&)>& callback);
	void SetMouseButtonCallback(const std::function<void(Window&, int, int, int)>& callback);
	void SetMouseScrollCallback(const std::function<void(Window&, float)>& callback);

private:
	std::string m_title;
	void* m_nativeWindowPtr;
	unsigned int m_width, m_height;
	bool m_fullscreen, m_vsync;

	// callbacks

	std::function<void(Window&)> m_closeCallback;
	std::function<void(Window&, int, int)> m_resizeCallback;
	std::function<void(Window&, int, const char**)> m_filedropCallback;
	std::function<void(Window&, bool)> m_focusCallback;
	std::function<void(Window&, int, int, int, int)> m_keyCallback;
	std::function<void(Window&, const glm::vec2&)> m_mousePosCallback;
	std::function<void(Window&, int, int, int)> m_mouseButtonCallback;
	std::function<void(Window&, float)> m_mouseScrollCallback;
};