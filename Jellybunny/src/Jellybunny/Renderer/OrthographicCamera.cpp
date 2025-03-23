#include "jbpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Jellybunny
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1000.0f, 1000.0f)), m_ViewMatrix(glm::mat4(1.0f))
	{
		JB_PROFILE_FUNCTION();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		JB_PROFILE_FUNCTION();
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1000.0f, 1000.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		JB_PROFILE_FUNCTION();
		GetPosition();
		m_ViewMatrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(m_Trans, glm::radians(-m_Rotation.x), glm::vec3(1, 0, 0)), glm::radians(-m_Rotation.y), glm::vec3(0, 1, 0)), glm::radians(-m_Rotation.z), glm::vec3(0, 0, 1)), -m_Position);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}