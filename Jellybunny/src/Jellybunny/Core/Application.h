#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"
#include "Jellybunny/Event/Event.h"
#include "Jellybunny/Event/ApplicationEvent.h"

#include "Jellybunny/Core/Timestep.h"

#include "Jellybunny/ImGui/ImGuiLayer.h"

#include "Jellybunny/Renderer/Shader.h"
#include "Jellybunny/Renderer/Buffer.h"
#include "Jellybunny/Renderer/VertexArray.h"
#include "Jellybunny/Renderer/OrthographicCamera.h"

namespace Jellybunny
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			JB_CORE_ASS(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string name = "Jellybunny Application";
		std::string workingDirectory;
		ApplicationCommandLineArgs commandLineArgs;
	};


	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		void Die();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationSpecification m_Specification;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrame = 0.0f;
	private:
		static Application* s_Instance;
	};

	// defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}