#include "Core/Input.h"
#include <GLFW/glfw3.h>
#include <string.h>

/* INPUT CLASS */

Input::Input()
{
	m_window = nullptr;
	m_mouseInputMode = MouseInputMode::CursorNormal;
	m_mousePos = { 0.0f, 0.0f };
	m_scrollDelta = 0.0f;
	
	for (int i = 0; i < NUM_KEYS; i++)
		m_keys[i] = GLFW_RELEASE;

	for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
		m_buttons[i] = GLFW_RELEASE;
}

Input& Input::Get()
{
	static Input instance;
	return instance;
}

void Input::SetFocusWindow(Window& window)
{
	m_window = &window;

	// set key callback

	window.SetKeyCallback([this](Window& window, int key, int scanCode, int action, int mods) {
		switch (action)
		{
		case GLFW_RELEASE:
			m_keys[key] = GLFW_RELEASE;
			break;
		case GLFW_REPEAT:
		case GLFW_PRESS:
			m_keys[key] = GLFW_PRESS;
			break;
		}
	});

	// set mouse button callback

	window.SetMouseButtonCallback([this](Window& window, int button, int action, int mods) {
		switch (action)
		{
		case GLFW_RELEASE:
			m_buttons[button] = GLFW_RELEASE;
			break;
		case GLFW_PRESS:
			m_buttons[button] = GLFW_PRESS;
			break;
		}
	});

	// set mouse position callback

	window.SetMousePosCallback([this](Window& window, const glm::vec2& mousePosition) {
		m_mousePos = mousePosition;
	});

	// set mouse scroll callback

	window.SetMouseScrollCallback([this](Window& window, float delta) {
		m_scrollDelta = delta;
	});
}

void Input::SetMouseInputMode(MouseInputMode mode)
{
	int glfwMode = GLFW_CURSOR_NORMAL;
	m_mouseInputMode = mode;

	switch (mode)
	{
	case MouseInputMode::CursorNormal:
		glfwMode = GLFW_CURSOR_NORMAL;
		break;
	case MouseInputMode::CursorDisabled:
		glfwMode = GLFW_CURSOR_DISABLED;
		break;
	}

	glfwSetInputMode((GLFWwindow*)m_window->GetNativeWindowPtr(), GLFW_CURSOR, glfwMode);
}

void Input::Update()
{
	m_scrollDelta = 0.0f;
}

bool Input::IsKeyPressed(Key key) const
{
	return m_keys[(int)key] == GLFW_PRESS;
}

bool Input::IsKeyReleased(Key key) const
{
	return m_keys[(int)key] == GLFW_RELEASE;
}

bool Input::IsMousePressed(MouseButton button) const
{
	return m_buttons[(int)button] == GLFW_PRESS;
}

bool Input::IsMouseReleased(MouseButton button) const
{
	return m_buttons[(int)button] == GLFW_RELEASE;
}

float Input::GetMouseScrollDelta() const
{
	return m_scrollDelta;
}