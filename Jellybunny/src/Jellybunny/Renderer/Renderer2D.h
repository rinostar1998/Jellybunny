#pragma once
#include "OrthographicCamera.h"

#include "Texture.h"
#include "SubTexture2D.h"

#include "Jellybunny/Renderer/Camera.h"
#include "Jellybunny/Renderer/EditorCamera.h"
#include "Jellybunny/Scene/Components.h"

namespace Jellybunny
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Die();

		static void BeginScene(const Camera& camera, glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawSprite(const glm::mat4& transform, SpriteComponent& sprite, int entityID);

		// Apes (Primitives) (Rotation in deg)
		static void DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color);
		static void DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color);
		static void DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1 );
		static void DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<SubTexture2D>& subtexture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<SubTexture2D>& subtexture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f));

		static void DrawRect(const glm::mat4& transform, const Ref<Texture2D>& texture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, const float uvScale = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), float thickness = 1.0f, float softness = 0.005f, int entityID = -1);
		static void DrawLine  (const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color, int entityID = -1);
		static void DrawWireframeRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4& color, int entityID = -1);
		static void DrawWireframeRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static float GetLineThickness();
		static void SetLineThickness(float thickness);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();
	private:
		static void StartBatch();
		static void FlushAndReset();
	};
}