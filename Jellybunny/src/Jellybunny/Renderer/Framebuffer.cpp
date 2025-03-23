#include "jbpch.h"
#include "Framebuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Jellybunny
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecs& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGlFramebuffer>(spec);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}

}