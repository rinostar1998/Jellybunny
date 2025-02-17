#include "jbpch.h"
#include "OpenGlShader.h"

#include <fstream>
#include <filesystem>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Jellybunny
{

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") { return GL_VERTEX_SHADER; }
		if (type == "fragment" || type == "pixel") { return GL_FRAGMENT_SHADER; }

		JB_CORE_ASS(false, "WHAT TYPE OF SORCERY SHADER IS THIS CRAP!???");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		JB_PROFILE_FUNCTION();
		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// ass/shader/texture.glsl
		std::filesystem::path path = filePath;
		m_Name = path.stem().string();
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertSource, const std::string& fragSource) : m_Name(name)
	{
		JB_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertSource;
		sources[GL_FRAGMENT_SHADER] = fragSource;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		JB_PROFILE_FUNCTION();
		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		JB_PROFILE_FUNCTION();
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			JB_CORE_ERR("COULD NOT OPEN UR SHITTY ASS FILE : '{0}'", filePath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		JB_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> sources;

		const char* typeToken = "#type";
		size_t typeTokenLen = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			JB_CORE_ASS(eol != std::string::npos, "A FUCKING SYNTAX ERROR FOR YOU TO FIGURE OUT~!");
			size_t begin = pos + typeTokenLen + 1;
			std::string type = source.substr(begin, eol - begin);
			JB_CORE_ASS(ShaderTypeFromString(type), "WHAT DRUG WERE YOU ON, TRYING TO USE AN INVALID TYPE SPECIFIER???");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			sources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}
		return sources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		JB_PROFILE_FUNCTION();
		GLuint program = glCreateProgram();
		JB_CORE_ASS(shaderSources.size() <= 2, "WHY THE FUCK DO YOU NEED MORE THAN 2 SHADERS!??? NO, I'M NOT GONNA ALLOCATE THAT!!");
		std::array<GLenum, 2> glShaderIds;
		int glShaderIdIdx = 0;
		for (auto& kv : shaderSources)
		{
			GLenum shaderType = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(shaderType);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				JB_CORE_ERR("{0}", infoLog.data());
				JB_CORE_ASS(false, "HOW THE HELL DID YOU FUCK UP THE SHADER!???");
				break;
			}
			glAttachShader(program, shader);
			glShaderIds[glShaderIdIdx++] = shader;
		}
		
		m_RendererID = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);


			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);


			glDeleteProgram(program);

			for(auto id : glShaderIds)
				glDeleteShader(id);

			JB_CORE_ERR("{0}", infoLog.data());
			JB_CORE_ASS(false, "~hyehehe... HAHAHAH!!! LINKER ERROR!!! FUCK YOU!!! \n SOME UNEXPLANIBLE ERROR MESSAGE: *#&RW*&#$@#*@ @UH$@UHFSUFHD in funtion @$&(@*()");
			return;
		}

		for (auto id : glShaderIds)
			glDetachShader(program, id);

		m_RendererID = program;
	}

	void OpenGLShader::Bind() const
	{
		JB_PROFILE_FUNCTION();
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		JB_PROFILE_FUNCTION();
		glUseProgram(0);
	}

	void OpenGLShader::SetMat2(const std::string& name, const glm::mat2& matrix)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformMat2(name, matrix);
	}

	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& matrix)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformMat3(name, matrix);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformMat4(name, matrix);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetInt2(const std::string& name, const glm::ivec2& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformInt2(name, value);
	}

	void OpenGLShader::SetInt3(const std::string& name, const glm::ivec3& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformInt3(name, value);
	}

	void OpenGLShader::SetInt4(const std::string& name, const glm::ivec4& value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformInt4(name, value);
	}

	void OpenGLShader::SetInt(const std::string& name, const int value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetFloat(const std::string& name, const float value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetBool(const std::string& name, const bool value)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformBool(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, const int* values, uint32_t count)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloatArray(const std::string& name, const float* values, uint32_t count)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformFloatArray(name, values, count);
	}

	void OpenGLShader::SetBoolArray(const std::string& name, const bool* values, uint32_t count)
	{
		JB_PROFILE_FUNCTION();
		UploadUniformBoolArray(name, values, count);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, const int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformInt2(const std::string& name, const glm::ivec2& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2i(location, vector.x, vector.y);
	}

	void OpenGLShader::UploadUniformInt3(const std::string& name, const glm::ivec3& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3i(location, vector.x, vector.y, vector.z);
	}

	void OpenGLShader::UploadUniformInt4(const std::string& name, const glm::ivec4& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4i(location, vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, vector.x, vector.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, vector.x, vector.y, vector.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vector)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGLShader::UploadUniformMat2(const std::string& name, const glm::mat2& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformBool(const std::string& name, const bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1fv(location, count, values);
	}

	void OpenGLShader::UploadUniformBoolArray(const std::string& name, const bool* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, (int*)values);
	}

}