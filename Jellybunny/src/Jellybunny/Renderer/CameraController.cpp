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
		JB_PROFILE_FUNCTION();
		if (Input::IsKeyPressed(JB_KEY_A))
		{
			m_CamPos.x -= cos(glm::radians(m_CamRot.z)) *  m_CamSpeed * time.DeltaTime();
			m_CamPos.y -= sin(glm::radians(m_CamRot.z)) *  m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_D))
		{
			m_CamPos.x += cos(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
			m_CamPos.y += sin(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_W))
		{
			m_CamPos.x -= sin(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
			m_CamPos.y += cos(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
		}
		if (Input::IsKeyPressed(JB_KEY_S))
		{
			m_CamPos.x += sin(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
			m_CamPos.y -= cos(glm::radians(m_CamRot.z)) * m_CamSpeed * time.DeltaTime();
		}

		auto [x, y] = Input::GetMousePosition();
		m_CamRot.z = x * 0.1f;

		m_CamRot.x = (m_CamRot.x > 360.0f ? m_CamRot.x - 360.0f : m_CamRot.x);
		m_CamRot.x = (m_CamRot.x < 0.0f ? m_CamRot.x + 360.0f : m_CamRot.x);
		m_CamRot.y = (m_CamRot.y > 360.0f ? m_CamRot.y - 360.0f : m_CamRot.y);
		m_CamRot.y = (m_CamRot.y < 0.0f ? m_CamRot.y + 360.0f : m_CamRot.y);
		m_CamRot.z = (m_CamRot.z > 360.0f ? m_CamRot.z - 360.0f : m_CamRot.z);
		m_CamRot.z = (m_CamRot.z < 0.0f ? m_CamRot.z + 360.0f : m_CamRot.z);

		m_CamSpeed = std::sqrt(m_Zoom);
		m_Camera.SetPosition(m_CamPos);
		m_Camera.SetRotation(m_CamRot);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		JB_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(JB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(JB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		JB_PROFILE_FUNCTION();
		m_Zoom -= e.GetYOffset() * 0.25f;
		m_Zoom = m_Zoom >= 0.25f ? m_Zoom : 0.25f;
		m_Camera.SetProjection(-m_aspectRatio * m_Zoom, m_aspectRatio * m_Zoom, -m_Zoom, m_Zoom);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		JB_PROFILE_FUNCTION();
		m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_aspectRatio * m_Zoom, m_aspectRatio * m_Zoom, -m_Zoom, m_Zoom);
		return false;
	}

}