#pragma once
#include "Jellybunny/Renderer/Camera.h"
#include "Jellybunny/Core/Timestep.h"

#include "Jellybunny/Event/ApplicationEvent.h"
#include "Jellybunny/Event/MouseEvent.h"

namespace Jellybunny
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio); // aspectRation * 2.0 units

		void OnUpdate(Timestep time);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_aspectRatio;
		float m_Zoom = 1.0f;
		OrthographicCamera m_Camera;
		glm::vec3 m_CamPos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_CamRot = { 0.0f, 0.0f, 0.0f };

		float m_CamSpeed = 1.0f;
	};
}