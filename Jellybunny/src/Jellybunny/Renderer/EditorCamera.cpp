#include "jbpch.h"
#include "EditorCamera.h"

#include "Jellybunny/Core/Input.h"
#include "Jellybunny/Core/KeyCodes.h"
#include "Jellybunny/Core/MouseCodes.h"
#include "Jellybunny/Core/Input.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Jellybunny {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
		m_WalkPos = CalculatePosition();
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		useWalkMode = Input::IsKeyPressed(Key::LeftShift);
		if (Input::IsKeyPressed(Key::LeftAlt) && !useWalkMode)
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(2))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(0))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(1))
				MouseZoom(delta.y);

			UpdateView();
		}
		if (useWalkMode)
		{
			Input::CaptureMouseCursor();
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			MouseRotate(-delta);
			float forwardMovInput = ((int)Input::IsKeyPressed(Key::W) - (int)Input::IsKeyPressed(Key::S)) * 0.25f;
			float forwardSideInput = ((int)Input::IsKeyPressed(Key::D) - (int)Input::IsKeyPressed(Key::A)) * 0.25f;

			glm::quat orientation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
			glm::vec3 forward = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
			glm::vec3 right = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));

			m_Position.x += forward.x * forwardMovInput;
			m_Position.y += forward.y * forwardMovInput;
			m_Position.z += forward.z * forwardMovInput;

			m_Position.x += right.x * forwardSideInput;
			m_Position.y += right.y * forwardSideInput;
			m_Position.z += right.z * forwardSideInput;

			m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
			m_ViewMatrix = glm::inverse(m_ViewMatrix);

			m_FocalPoint = m_Position + (GetForwardDirection() * m_Distance);
		}
		else
		{
			Input::ReleaseMouseCursor();
		}
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		if(!useWalkMode) dispatcher.Dispatch<MouseScrolledEvent>(JB_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::mat4 EditorCamera::GetTransform()
	{
		static glm::mat4 trans = glm::mat4(1.0f);
		return glm::inverse(glm::translate(trans, m_WalkPos) * glm::rotate(trans, m_Yaw, { 1, 0, 0 }) * glm::rotate(trans, m_Pitch, { 0, 1, 0 }));
	}

}