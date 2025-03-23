#pragma once

#include "Jellybunny.h"

class Sandbox2D : public Jellybunny::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Jellybunny::Timestep time) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Jellybunny::Event& e) override;
private:
	Jellybunny::OrthographicCameraController m_CameraController;

	Jellybunny::Ref<Jellybunny::Shader> m_FlatColorShader;
	Jellybunny::Ref<Jellybunny::VertexArray> m_VertexArray;
	Jellybunny::Ref<Jellybunny::VertexBuffer> m_VertexBuffer;
	Jellybunny::Ref<Jellybunny::IndexBuffer> m_IndexBuffer;
	Jellybunny::Ref<Jellybunny::Texture2D> m_Texture;

	Jellybunny::Ref<Jellybunny::Texture2D> m_SpriteSheet;
	Jellybunny::Ref<Jellybunny::SubTexture2D> m_TextureSub;

	glm::vec4 m_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Jellybunny::Ref<Jellybunny::SubTexture2D>> s_TextureMap;
};