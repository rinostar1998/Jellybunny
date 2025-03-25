#include "jbpch.h"
#include "Jellybunny/Core/Application.h"

#include "Jellybunny/Core/Log.h"

#include "Jellybunny/Renderer/Renderer.h"
#include "Jellybunny/Scripting/ScriptModule.h"

#include "Jellybunny/Core/Input.h"
#include <filesystem>
#include <glfw/glfw3.h>

namespace Jellybunny
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification) : m_Specification(specification)
	{
		JB_PROFILE_FUNCTION();
		JB_CORE_ASS(!s_Instance, "APPLICATION ALREADY EXISTS!");
		s_Instance = this;

		// Create working dir here
		if(!m_Specification.workingDirectory.empty()) std::filesystem::current_path(m_Specification.workingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.name));
		m_Window->SetEventCallback(JB_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		ScriptModule::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		JB_PROFILE_FUNCTION();
		ScriptModule::Die();
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
		dispatcher.Dispatch<WindowCloseEvent>(JB_BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(JB_BIND_EVENT_FN(OnWindowResize));

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

	void Application::Die()
	{
		m_Running = false;
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