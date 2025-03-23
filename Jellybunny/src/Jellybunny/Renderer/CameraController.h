#pragma once
#include "Jellybunny/Renderer/OrthographicCamera.h"
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

		void OnResize(float width, float height);

		void SetZoomLevel(const float zoom) { m_Zoom = zoom; CalculateView(); }
		float GetZoomLevel() { return m_Zoom; }
		void SetrotationControlEnabled(bool enabled) { m_Rotate = enabled; }

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		void CalculateView();
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_aspectRatio;
		float m_Zoom = 1.0f;
		OrthographicCamera m_Camera;
		glm::vec3 m_CamPos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_CamRot = { 0.0f, 0.0f, 0.0f };

		float m_Width = 720.0f;

		float m_CamSpeed = 1.0f;
		bool m_Rotate = false;
	};
}