#include "jbpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Jellybunny
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}
}