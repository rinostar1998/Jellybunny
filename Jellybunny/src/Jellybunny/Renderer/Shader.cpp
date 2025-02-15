#include "jbpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Jellybunny
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filePath);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  JB_CORE_ASS(false, "WHERE'D YOU BUY SUCH A SHITTY COMPUTER TO NOT HAVE A GRAPHICS API!?!?!? (RendererAPI::None not supported!)"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSource, fragmentSource);
		}
		JB_CORE_ASS(false, "WHAT BLACK MARKET DID YA GET THIS COMPUTER FROM!? I DON'T EVEN KNOW WHAT GRAPHICS API IT SUPPORTS!!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		JB_CORE_ASS(!isValid(name), "YOUR FUCKING SHADER WAS ALREADY ADDED, DID YA FORGET!???");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}


	Jellybunny::Ref<Jellybunny::Shader> ShaderLibrary::Load(const std::string& name)
	{
		auto shader = Shader::Create(name);
		Add(shader);
		return shader;
	}

	Jellybunny::Ref<Jellybunny::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::Create(name);
		Add(name, shader);
		return shader;
	}

	Jellybunny::Ref<Jellybunny::Shader> ShaderLibrary::Get(const std::string& name)
	{
		JB_CORE_ASS(isValid(name), "YA REALLY THOUGH I WOULD HALLUCINATE CREATING THE SHADER FIRST, HUH!????");
		return m_Shaders[name];
	}

	bool ShaderLibrary::isValid(const std::string& name) const
	{
		return m_Shaders.find(name) == m_Shaders.end();
	}

}