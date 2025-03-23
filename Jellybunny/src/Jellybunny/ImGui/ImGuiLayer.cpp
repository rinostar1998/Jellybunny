#include "jbpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Jellybunny/Core/Application.h"

// temp
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ImGuizmo.h"


namespace Jellybunny {

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::OnAttach()
	{
		JB_PROFILE_FUNCTION();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("assets/fonts/inconsolata/Inconsolata-Bold.ttf", 14.0f);
		io.FontDefault =  io.Fonts->AddFontFromFileTTF("assets/fonts/inconsolata/Inconsolata-Regular.ttf", 14.0f);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		JB_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		JB_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		JB_PROFILE_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.02f, 0.01f, 0.02f, 1.0f };

		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.01f, 0.02f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.4f, 0.08f, 0.1f, 1.0f };

		colors[ImGuiCol_Button] = ImVec4{ 0.08f, 0.08f, 0.2f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.1f, 0.12f, 0.4f, 1.0f };

		colors[ImGuiCol_Tab] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.2f, 0.0f, 0.0f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.5f, 0.08f, 0.0f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.0f, 0.04f, 1.0f };

		colors[ImGuiCol_TitleBg] = ImVec4{ 0.05f, 0.0f, 0.0f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.3f, 0.05f, 0.02f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.0f, 0.0f, 0.1f, 1.0f };
	}
}