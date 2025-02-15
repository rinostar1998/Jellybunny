#include "jbpch.h"
#include "CameraController.h"

#include "Jellybunny/Core/Input.h"
#include "Jellybunny/Core/KeyCodes.h"

namespace Jellybunny
{

	OrthographicCameraController::OrthographicCameraController(float aspectRatio) : m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_Zoom, m_aspectRatio * m_Zoom, -m_Zoom, m_Zoom)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep time)
	{

		if (Input::IsKeyPressed(JB_KEY_A))
		{
			m_CamPos.x -= m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_D))
		{
			m_CamPos.x += m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_W))
		{
			m_CamPos.y += m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_S))
		{
			m_CamPos.y -= m_CamSpeed * time.DeltaTime();
		}

		m_CamSpeed = std::sqrt(m_Zoom);
		m_Camera.SetPosition(m_CamPos);
		m_Camera.SetRotation(m_CamRot);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(JB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(JB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_Zoom -= e.GetYOffset() * 0.25f;
		m_Zoom = m_Zoom >= 0.25f ? m_Zoom : 0.25f;
		m_Camera.SetProjection(-m_aspectRatio * m_Zoom, m_aspectRatio * m_Zoom, -m_Zoom, m_Zoom);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_aspectRatio * m_Zoom, m_aspectRatio * m_Zoom, -m_Zoom, m_Zoom);
		return false;
	}

}