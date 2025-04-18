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

		virtual void SetMat2(const std::string& name, const glm::mat2& matrix) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) override;
		virtual void SetInt3(const std::string& name, const glm::ivec3& value) override;
		virtual void SetInt4(const std::string& name, const glm::ivec4& value) override;
		virtual void SetInt(const std::string& name, const int value) override;
		virtual void SetFloat(const std::string& name, const float value) override;
		virtual void SetBool(const std::string& name, const bool value) override;

		virtual void SetIntArray(const std::string& name, const int* values, uint32_t count) override;
		virtual void SetFloatArray(const std::string& name, const float* values, uint32_t count) override;
		virtual void SetBoolArray(const std::string& name, const bool* values, uint32_t count) override;

		virtual const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt2(const std::string& name, const glm::ivec2& vector);
		void UploadUniformInt3(const std::string& name, const glm::ivec3& vector);
		void UploadUniformInt4(const std::string& name, const glm::ivec4& vector);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vector);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vector);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vector);
		void UploadUniformMat2(const std::string& name, const glm::mat2& matrix);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformInt(const std::string& name, const int value);
		void UploadUniformFloat(const std::string& name, const float value);
		void UploadUniformBool(const std::string& name, const bool value);

		void UploadUniformIntArray(const std::string& name, const int* values, uint32_t count);
		void UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count);
		void UploadUniformBoolArray(const std::string& name, const bool* values, uint32_t count);
	private:
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileOrGetOpenGLBinaries();
		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}