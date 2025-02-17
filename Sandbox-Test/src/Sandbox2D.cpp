#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	JB_PROFILE_FUNCTION();
	m_Texture = Jellybunny::Texture2D::Create("assets/textures/FrownExtraMad.png");
}

void Sandbox2D::OnDetach()
{
	JB_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Jellybunny::Timestep time)
{
	JB_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(time);

	// Render
	Jellybunny::Renderer2D::ResetStats();
	{
		JB_PROFILE_SCOPE("Renderer Update Init");
		Jellybunny::RenderCommand::SetClearColor({ 0.1f, 0, 0, 1 });
		Jellybunny::RenderCommand::Clear();
	}

	{
		JB_PROFILE_SCOPE("Renderer Draw Scene");
		Jellybunny::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Jellybunny::Renderer2D::DrawRect({ 0.0f,  0.0f }, 45.0f, { 1.0f, 1.0f }, m_Color);
		Jellybunny::Renderer2D::DrawRect({ 2.0f, -0.5f }, -20.0f, { 1.0f, 2.0f }, { m_Color.b, m_Color.g, m_Color.r, m_Color.a });
		Jellybunny::Renderer2D::DrawRect({ 2.0f, -0.5f , -0.1f }, -80.0f, { 20.0f, 20.0f }, m_Texture, 10.0f, { m_Color.g, m_Color.r, m_Color.b, 1.0f });
		Jellybunny::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	JB_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Peepee Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void Sandbox2D::OnEvent(Jellybunny::Event& e)
{
	m_CameraController.OnEvent(e);
}