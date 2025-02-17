#include "jbpch.h"
#include "Application.h"

#include "Jellybunny/Core/Log.h"

#include "Jellybunny/Renderer/Renderer.h"

#include "Input.h"
#include <glfw/glfw3.h>

namespace Jellybunny
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		JB_CORE_ASS(s_Instance, "APPLICATION ALREADY EXISTS!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		JB_PROFILE_FUNCTION();
		Renderer::Die();
	}

	void Application::PushLayer(Layer* layer)
	{
		JB_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		JB_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		JB_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		JB_PROFILE_FUNCTION();
		while (m_Running)
		{
			JB_PROFILE_SCOPE("RUN LOOP");
			float time = (float)glfwGetTime();
			Timestep timestep = Timestep(time - m_LastFrame, time);
			m_LastFrame = time;

			if (!m_Minimized)
			{
				{
					JB_PROFILE_SCOPE("LAYERSTACK OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
				m_ImGuiLayer->Begin();
				{
					JB_PROFILE_SCOPE("LAYERSTACK OnImGuiRender");
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		JB_PROFILE_FUNCTION();
		if (e.GetWidth() == 0 || e.GetHeight() == 0) { m_Minimized = true;  return false; }
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

}