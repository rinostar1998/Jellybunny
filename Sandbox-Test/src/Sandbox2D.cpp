#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGBBBBBBBBBBBBBBGGGGG"
"GGGGBBBBBBBBBBBBBBGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGCCCCCCGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG"
"GGGGGGGGGGGGGGGGGGGGGGG";

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	JB_PROFILE_FUNCTION();
	m_Texture = Jellybunny::Texture2D::Create("assets/textures/FrownExtraMad.png");
	m_SpriteSheet = Jellybunny::Texture2D::Create("assets/game/textures/tilemap_packed.png");
	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
	s_TextureMap['G'] = Jellybunny::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 11}, {16, 16}, {1, 1});
	s_TextureMap['B'] = Jellybunny::SubTexture2D::CreateFromCoords(m_SpriteSheet, {4, 6}, {16, 16}, {1, 1});
	s_TextureMap['C'] = Jellybunny::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 0}, {16, 16}, {1, 1});

	m_CameraController.SetZoomLevel(5.0f);
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

	Jellybunny::Renderer2D::ResetStats();
	{
		JB_PROFILE_SCOPE("Renderer Update Init");
		Jellybunny::RenderCommand::SetClearColor({ 0.1f, 0, 0, 1 });
		Jellybunny::RenderCommand::Clear();
	}

	// Render
	{
		JB_PROFILE_SCOPE("Renderer Draw Scene");
		Jellybunny::Renderer2D::BeginScene(m_CameraController.GetCamera());


		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];
				Jellybunny::Ref<Jellybunny::SubTexture2D> texture;
				if (s_TextureMap.find(tileType) != s_TextureMap.end())
					texture = s_TextureMap[tileType];
				else
					texture = s_TextureMap['D'];

				Jellybunny::Renderer2D::DrawRect({ x, y, 0.0f }, 0.0f, { 1.0f, 1.0f }, texture, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
		}
		Jellybunny::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	JB_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Jellybunny::Renderer2D::GetStats();
	ImGui::Text("Renderer2D : Stats");
	ImGui::Text("DRAW CALLS : %d", stats.DrawCalls);
	ImGui::Text("QUAD COUNT : %d", stats.QuadCount);
	ImGui::Text("VERT COUNT : %d", stats.GetTotalVertexCount());
	ImGui::Text("INDX COUNT : %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Peepee Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void Sandbox2D::OnEvent(Jellybunny::Event& e)
{
	m_CameraController.OnEvent(e);
}