#include "OpenglApplication.h"
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "Core/Input.h"
#include "Core/Graphics/Renderer2D.h"
#include "Core/Graphics/RenderCommand.h"

/* OpenglApplication */

OpenglApplication::OpenglApplication()
{
	m_running = false;
}

bool OpenglApplication::Init(const WindowSpecs& windowSpecs)
{
	// init

	if (!m_window.Create(windowSpecs)) // create window, if not succesfully return false
		return false;

	m_running = true;

	// set window callbacks

	m_window.SetCloseCallback([this](Window& window) {
		m_running = false;
	});

	// set input focus window

	Input::Get().SetFocusWindow(m_window);

	// display Renderer2D info

	std::cout << "Renderer2D: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

	/* IMGUI INIT */

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Dockings
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/FiraCode-SemiBold.ttf", 23.0f); // set default font
	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_window.GetNativeWindowPtr(), true);
	ImGui_ImplOpenGL3_Init("#version 450");

	/* IMGUI THEME */

	SetImGuiTheme();

	// create Renderer2D

	Renderer2D::Get();

	// call on start

	OnStart();

	return true;
}

OpenglApplication::~OpenglApplication()
{
	// shutdown imgui

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OpenglApplication::OnStart()
{

}

void OpenglApplication::Update(float delta)
{

}

void OpenglApplication::FixedUpdate(float delta)
{
	m_macroPadController.Update(delta);
}

void OpenglApplication::Render()
{	
	RenderCommand::Clear({ 0.15f, 0.15f, 0.15f, 1.0f });
}

void OpenglApplication::RenderImGui()
{
	ImGui::ShowDemoWindow();

	m_macroPadController.RenderImGui();
}

void OpenglApplication::Run()
{
	// timer control variables

	const double fixedStep = 1.0 / 60.0;

	double timeBefore = glfwGetTime();
	double timeAfter = 0;
	double delta = 0;
	double fixedDelta = 0;
	double fpsTimer = 0;
	int fpsCounter = 0;

	while (m_running)
	{
		// poll events

		glfwPollEvents();

		// update

		Update((float)delta);

		// fixed update

		if (fixedDelta >= fixedStep)
		{
			FixedUpdate((float)fixedStep);

			fixedDelta -= fixedStep;
		}

		Input::Get().Update(); // input update

		/* RENDER */

		Renderer2D::Get().ResetStats(); // reset Renderer2D stats first
		Render();

		/* RENDER IMGUI */

		// prepare to render imgui

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// render imgui

		RenderImGui();

		// flush imgui

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* SWAP WINDOW BUFFERS */

		m_window.SwapBuffers();

		// frame done

		fpsCounter++;

		// get the delta time

		timeAfter = glfwGetTime();
		delta = timeAfter - timeBefore;
		fixedDelta += delta;
		fpsTimer += delta;
		timeBefore = timeAfter;

		// print the fps every second

		if (fpsTimer >= 1.0)
		{
			// std::string title = "Game Title, " + std::to_string(fpsCounter) + " fps";
			char title[30];
			snprintf(title, 30, "Game Title, %d fps", fpsCounter);
			m_window.SetTitle(title);

			fpsTimer = 0;
			fpsCounter = 0;
		}
	}
}

void OpenglApplication::Stop()
{
	m_running = false;
}
