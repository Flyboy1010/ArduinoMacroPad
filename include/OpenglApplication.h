#pragma once

#include "Core/Graphics/Window.h"
#include "ArduinoMacroPadController.h"

class OpenglApplication
{
public:
	OpenglApplication();
	~OpenglApplication();

	Window& GetWindow() { return m_window; }

	bool Init(const WindowSpecs& windowSpecs);
	void Run();
	void Stop();

	void SetImGuiTheme();

	void OnStart();
	void Update(float delta);
	void FixedUpdate(float delta);
	void Render();
	void RenderImGui();

private:
	Window m_window;
	bool m_running;

	ArduinoMacroPadController m_macroPadController;
};
