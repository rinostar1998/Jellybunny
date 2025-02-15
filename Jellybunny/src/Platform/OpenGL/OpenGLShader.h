#pragma once

#include "Jellybunny/Renderer/Shader.h"
#include <glm/glm.hpp>

//TODO: Remove
typedef unsigned int GLenum;

namespace Jellybunny
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertSource, const std::string& fragSource);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(const std::string& name, const int value);
		void UploadUniformInt2(const std::string& name, const glm::vec2& vector);
		void UploadUniformInt3(const std::string& name, const glm::vec3& vector);
		void UploadUniformInt4(const std::string& name, const glm::vec4& vector);
		void UploadUniformFloat(const std::string& name, const float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vector);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vector);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vector);
		void UploadUniformMat2(const std::string& name, const glm::mat2& matrix);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}