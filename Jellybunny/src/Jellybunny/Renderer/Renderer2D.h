#pragma once
#include "Camera.h"
#include "Texture.h"

namespace Jellybunny
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Die();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// Apes (Primitives)
		static void DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color);
		static void DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color);
		static void DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale = 1.0f, const glm::vec4 color = glm::vec4(1.0f));
		static void DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale = 1.0f, const glm::vec4 color = glm::vec4(1.0f));
		
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		Statistics GetStats();
		static void ResetStats();
	};
}