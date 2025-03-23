#include "jbpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Jellybunny/Renderer/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Jellybunny
{
	struct RectVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float uvScale;

		// EDITOR ONLY
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// EDITOR ONLY
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// EDITOR ONLY
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxRects = 20000;
		static const uint32_t MaxVerts = MaxRects * 4;
		static const uint32_t MaxIndices = MaxRects * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> RectVertexArray;
		Ref<VertexBuffer> RectVertexBuffer;
		Ref<Shader> RectShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t RectIndexCount = 0;
		RectVertex* RectVertBufferBase = nullptr;
		RectVertex* RectVertBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertBufferBase = nullptr;
		CircleVertex* CircleVertBufferPtr = nullptr;

		uint32_t LineVertCount = 0;
		LineVertex* LineVertBufferBase = nullptr;
		LineVertex* LineVertBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 rectVert[4] =
		{
			{ -0.5f, -0.5f, 0.0f, 1.0f},
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};
		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;


	void Renderer2D::Init()
	{
		JB_PROFILE_FUNCTION();
		s_Data.RectVertexArray = VertexArray::Create();

		s_Data.RectVertexBuffer = VertexBuffer::Create(s_Data.MaxVerts * sizeof(RectVertex));
		s_Data.RectVertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position"  },
				{ ShaderDataType::Float4, "a_Color"		},
				{ ShaderDataType::Float2, "a_TexCoord"  },
				{ ShaderDataType::Float, "a_TexIndex"	},
				{ ShaderDataType::Float, "a_UVScale"	},
				{ ShaderDataType::Int, "a_EntityID"		}
			});
		s_Data.RectVertexArray->AddVertexBuffer(s_Data.RectVertexBuffer);
		s_Data.RectVertBufferBase = new RectVertex[s_Data.MaxVerts];

		uint32_t* rectIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			rectIndices[i + 0] = offset + 0;
			rectIndices[i + 1] = offset + 1;
			rectIndices[i + 2] = offset + 2;

			rectIndices[i + 3] = offset + 2;
			rectIndices[i + 4] = offset + 3;
			rectIndices[i + 5] = offset + 0;
			offset += 4;
		}

		Ref<IndexBuffer> rectIndexBuffer;
		rectIndexBuffer = IndexBuffer::Create(rectIndices, s_Data.MaxIndices);
		s_Data.RectVertexArray->SetIndexBuffer(rectIndexBuffer);
		delete[] rectIndices;

		// Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVerts * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(rectIndexBuffer); // Use quad IB
		s_Data.CircleVertBufferBase = new CircleVertex[s_Data.MaxVerts];

		// Lines
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVerts * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertBufferBase = new LineVertex[s_Data.MaxVerts];

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t texData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&texData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++) samplers[i] = i;

		s_Data.RectShader = Shader::Create("assets/shaders/Renderer2D_Rect.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Die()
	{
		JB_PROFILE_FUNCTION();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.RectIndexCount = 0;
		s_Data.RectVertBufferPtr = s_Data.RectVertBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertBufferPtr = s_Data.CircleVertBufferBase;

		s_Data.LineVertCount = 0;
		s_Data.LineVertBufferPtr = s_Data.LineVertBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		JB_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, glm::mat4& transform)
	{
		JB_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		JB_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		JB_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.RectIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.RectVertBufferPtr - (uint8_t*)s_Data.RectVertBufferBase);
			s_Data.RectVertexBuffer->SetData(s_Data.RectVertBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.RectShader->Bind();
			RenderCommand::DrawIndexed(s_Data.RectVertexArray, s_Data.RectIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertBufferPtr - (uint8_t*)s_Data.CircleVertBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.LineVertCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertBufferPtr - (uint8_t*)s_Data.LineVertBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineThickness(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();
		s_Data.RectIndexCount = 0;
		s_Data.RectVertBufferPtr = s_Data.RectVertBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color)
	{
		DrawRect({ position.x, position.y, 0.0f }, rotation, scale, color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color)
	{
		JB_PROFILE_FUNCTION();

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if (rotation != 0) { rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });

		DrawRect(transform, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const Ref<Texture2D>& texture, const float uvScale, const glm::vec4& color, int entityID)
	{
		JB_PROFILE_FUNCTION();

		glm::vec2 rectTexCoord[4] =
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		if (s_Data.RectIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.RectVertBufferPtr->Position = (transform * s_Data.rectVert[i]);
			s_Data.RectVertBufferPtr->Color = color;
			s_Data.RectVertBufferPtr->TexCoord = rectTexCoord[i];
			s_Data.RectVertBufferPtr->TexIndex = textureIndex;
			s_Data.RectVertBufferPtr->uvScale = uvScale;
			s_Data.RectVertBufferPtr->EntityID = entityID;
			s_Data.RectVertBufferPtr++;
		}
		s_Data.RectIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		JB_PROFILE_FUNCTION();

		if (s_Data.RectIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float texIndex = 0.0f;
		const float uvScale = 1.0f;

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.RectVertBufferPtr->Position = (transform * s_Data.rectVert[i]);
			s_Data.RectVertBufferPtr->Color = color;
			s_Data.RectVertBufferPtr->TexIndex = 0;
			s_Data.RectVertBufferPtr->EntityID = entityID;
			s_Data.RectVertBufferPtr++;
		}

		s_Data.Stats.QuadCount++;
		s_Data.RectIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, const float uvScale, const glm::vec4& color)
	{
		JB_PROFILE_FUNCTION();

		const glm::vec2* rectTexCoord = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.RectIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.RectVertBufferPtr->Position = (transform * s_Data.rectVert[i]);
			s_Data.RectVertBufferPtr->Color = color;
			s_Data.RectVertBufferPtr->TexCoord = rectTexCoord[i];
			s_Data.RectVertBufferPtr->TexIndex = textureIndex;
			s_Data.RectVertBufferPtr->uvScale = uvScale;
			s_Data.RectVertBufferPtr++;
		}
		s_Data.RectIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale, const glm::vec4& color, int entityID)
	{
		DrawRect({ position.x, position.y, 0.0f }, rotation, scale, texture, uvScale, color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale, const glm::vec4& color, int entityID)
	{
		JB_PROFILE_FUNCTION();

		static glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if (rotation != 0) { rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });

		DrawRect(transform, texture, uvScale, color);
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<SubTexture2D>& subtexture, const float uvScale /*= 1.0f*/, const glm::vec4& color /*= glm::vec4(1.0f)*/)
	{
		DrawRect({ position.x, position.y, 0.0f }, rotation, scale, subtexture, uvScale, color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<SubTexture2D>& subtexture, const float uvScale /*= 1.0f*/, const glm::vec4& color /*= glm::vec4(1.0f)*/)
	{

		JB_PROFILE_FUNCTION();

		static glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if (rotation != 0) { rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });

		DrawRect(transform, subtexture, uvScale, color);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float softness, int entityID)
	{
		JB_PROFILE_FUNCTION();

		//if (s_Data.RectIndexCount >= Renderer2DData::MaxIndices)
			//FlushAndReset();

		const float texIndex = 0.0f;
		const float uvScale = 1.0f;

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertBufferPtr->WorldPosition = (transform * s_Data.rectVert[i]);
			s_Data.CircleVertBufferPtr->LocalPosition = s_Data.rectVert[i] * 2.0f;
			s_Data.CircleVertBufferPtr->Color = color;
			s_Data.CircleVertBufferPtr->Thickness = thickness;
			s_Data.CircleVertBufferPtr->Fade = softness;
			s_Data.CircleVertBufferPtr->EntityID = entityID;
			s_Data.CircleVertBufferPtr++;
		}
		s_Data.CircleIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color, int entityID)
	{
		s_Data.LineVertBufferPtr->Position = v1;
		s_Data.LineVertBufferPtr->Color = color;
		s_Data.LineVertBufferPtr->EntityID = entityID;
		s_Data.LineVertBufferPtr++;
		s_Data.LineVertBufferPtr->Position = v2;
		s_Data.LineVertBufferPtr->Color = color;
		s_Data.LineVertBufferPtr->EntityID = entityID;
		s_Data.LineVertBufferPtr++;

		s_Data.LineVertCount += 2;
	}

	void Renderer2D::DrawWireframeRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4& color, int entityID)
	{
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if (rotation != 0) { rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });
		glm::vec3 v1 = transform * s_Data.rectVert[0];
		glm::vec3 v2 = transform * s_Data.rectVert[1];
		glm::vec3 v3 = transform * s_Data.rectVert[2];
		glm::vec3 v4 = transform * s_Data.rectVert[3];
		DrawLine(v1, v2, color, entityID);
		DrawLine(v2, v3, color, entityID);
		DrawLine(v3, v4, color, entityID);
		DrawLine(v4, v1, color, entityID);
	}

	void Renderer2D::DrawWireframeRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 v1 = transform * s_Data.rectVert[0];
		glm::vec3 v2 = transform * s_Data.rectVert[1];
		glm::vec3 v3 = transform * s_Data.rectVert[2];
		glm::vec3 v4 = transform * s_Data.rectVert[3];
		DrawLine(v1, v2, color, entityID);
		DrawLine(v2, v3, color, entityID);
		DrawLine(v3, v4, color, entityID);
		DrawLine(v4, v1, color, entityID);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& sprite, int entityID)
	{
		if(sprite.Texture) DrawRect(transform, sprite.Texture, sprite.uvScale, sprite.Color);
		else DrawRect(transform, sprite.Color, entityID);
	}

	float Renderer2D::GetLineThickness()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineThickness(float thickness)
	{
		s_Data.LineWidth = thickness;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

}