#include "jbpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Jellybunny
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}