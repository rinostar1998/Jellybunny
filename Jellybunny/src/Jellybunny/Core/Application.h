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
#include "Jellybunny/Renderer/Camera.h"

namespace Jellybunny
{
	class JELLYBUNNY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
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
	Application* CreateApplication();
}