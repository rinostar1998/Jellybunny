#include "jbpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Jellybunny
{
	Ref<Texture2D> Texture2D::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(filePath);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}
}