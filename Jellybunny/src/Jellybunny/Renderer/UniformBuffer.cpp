#include "jbpch.h"
#include "UniformBuffer.h"

#include "Jellybunny/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Jellybunny {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    JB_CORE_ASS(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		JB_CORE_ASS(false, "Unknown RendererAPI!");
		return nullptr;
	}

}