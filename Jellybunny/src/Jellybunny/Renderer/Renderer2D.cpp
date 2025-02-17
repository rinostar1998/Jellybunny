#include "jbpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Jellybunny
{
	struct RectVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float uvScale;
	};

	struct Renderer2DData
	{
		const uint32_t MaxRects = 10000;
		const uint32_t MaxVerts = MaxRects * 4;
		const uint32_t MaxIndices = MaxRects * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> RectVertexArray;
		Ref<VertexBuffer> RectVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t RectIndexCount = 0;
		RectVertex* RectVertBufferBase = nullptr;
		RectVertex* RectVertBufferPtr = nullptr;
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_Data;


	void Renderer2D::Init()
	{
		JB_PROFILE_FUNCTION();
		s_Data.RectVertexArray = Jellybunny::VertexArray::Create();

		s_Data.RectVertexBuffer = Jellybunny::VertexBuffer::Create(s_Data.MaxVerts * sizeof(RectVertex));

		s_Data.RectVertexBuffer->SetLayout(
		{
			{ Jellybunny::ShaderDataType::Float3, "a_Position" },
			{ Jellybunny::ShaderDataType::Float4, "a_Color" },
			{ Jellybunny::ShaderDataType::Float2, "a_TexCoord" },
			{ Jellybunny::ShaderDataType::Float, "a_TexIndex" },
			{ Jellybunny::ShaderDataType::Float, "a_UVScale" }
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
		rectIndexBuffer = Jellybunny::IndexBuffer::Create(rectIndices, s_Data.MaxIndices);
		delete[] rectIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t texData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&texData, sizeof(uint32_t));
		s_Data.RectVertexArray->SetIndexBuffer(rectIndexBuffer);
		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
	}

	void Renderer2D::Die()
	{
		JB_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		JB_PROFILE_FUNCTION();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		s_Data.RectIndexCount = 0;
		s_Data.RectVertBufferPtr = s_Data.RectVertBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		JB_PROFILE_FUNCTION();
		uint32_t dataSize = (uint8_t*)s_Data.RectVertBufferPtr - (uint8_t*)s_Data.RectVertBufferBase;
		s_Data.RectVertexBuffer->SetData(s_Data.RectVertBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);
		RenderCommand::DrawIndexed(s_Data.RectVertexArray, s_Data.RectIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color)
	{
		DrawRect({ position.x, position.y, 0.0f }, rotation, scale, color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const glm::vec4 color)
	{
		JB_PROFILE_FUNCTION();

		const float texIndex = 0.0f;
		const float uvScale = 1.0f;

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if(rotation != 0){ rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ -0.5f, -0.5f, 0.0f, 1.0f}));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = texIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ 0.5f, -0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = texIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ 0.5f, 0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = texIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ -0.5f, 0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = texIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;
		s_Data.RectIndexCount += 6;

		s_Data.Stats.QuadCount++;

		/*s_Data.TextureShader->SetFloat("u_UVScale", 1.0f);
		s_Data.WhiteTexture->Bind();

		s_Data.TextureShader->SetMat4("u_Transform", transform);
		s_Data.RectVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.RectVertexArray);*/
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale, const glm::vec4 color)
	{
		DrawRect({ position.x, position.y, 0.0f }, rotation, scale, texture, uvScale, color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float& rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const float uvScale, const glm::vec4 color)
	{
		JB_PROFILE_FUNCTION();

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

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rotationMat = trans;
		if (rotation != 0) { rotationMat = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }); }
		glm::mat4 transform = glm::translate(trans, position) * rotationMat * glm::scale(trans, { scale.x, scale.y, 1.0f });
		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ -0.5f, -0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = textureIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ 0.5f, -0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.RectVertBufferPtr->TexIndex = textureIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ 0.5f, 0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.RectVertBufferPtr->TexIndex = textureIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.RectVertBufferPtr->Position = (transform * glm::vec4({ -0.5f, 0.5f, 0.0f, 1.0f }));
		s_Data.RectVertBufferPtr->Color = color;
		s_Data.RectVertBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.RectVertBufferPtr->TexIndex = textureIndex;
		s_Data.RectVertBufferPtr->uvScale = uvScale;
		s_Data.RectVertBufferPtr++;

		s_Data.Stats.QuadCount++;

		/*s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetFloat("u_UVScale", uvScale);
		texture->Bind();

		s_Data.RectVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.RectVertexArray);*/
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