#include "jbpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Jellybunny
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}
		JB_CORE_ASS(false, "HOW THE FUCK YOU GET AN UNKNOWN DATA TYPE TO GL TYPE CONVERSION ERROR!?");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		JB_PROFILE_FUNCTION();
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		JB_PROFILE_FUNCTION();
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		JB_PROFILE_FUNCTION();
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		JB_PROFILE_FUNCTION();
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer>& vertexBuffer)
	{
		JB_PROFILE_FUNCTION();
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		JB_CORE_ASS(vertexBuffer->GetLayout().GetElements().size(), "YA FORGOT A LITTLE SOMETHING... THE ENTIRE GODDAMN VERTEX BUFFER LAYOUT, PERHAPS!?!?");

		uint32_t i = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(i);
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glVertexAttribPointer(i, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset); // hehe... why r u here?
					i++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glVertexAttribIPointer(i, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(), (const void*)element.Offset); // hehe... why r u here?
					i++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t j = 0; j < count; j++)
					{
						glEnableVertexAttribArray(i);
						glVertexAttribPointer(i, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(element.Offset + sizeof(float) * count * i)); // hehe... why r u here?
						glVertexAttribDivisor(i, 1);
						i++;
					}
					break;
				}
				default:
					JB_CORE_ASS(false, "Strange Unidentified Shader Type, Asshole!");
			}
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer>& indexBuffer)
	{
		JB_PROFILE_FUNCTION();
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}