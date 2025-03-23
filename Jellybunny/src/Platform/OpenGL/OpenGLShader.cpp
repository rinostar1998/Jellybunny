#include "jbpch.h"
#include "OpenGlShader.h"

#include <fstream>
#include <filesystem>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


#include "Jellybunny/Core/Timer.h"


namespace Jellybunny
{

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			JB_CORE_ASS(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			JB_CORE_ASS(false);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			JB_CORE_ASS(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			JB_CORE_ASS(false);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			JB_CORE_ASS(false);
			return "";
		}
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") { return GL_VERTEX_SHADER; }
		if (type == "fragment" || type == "pixel") { return GL_FRAGMENT_SHADER; }

		JB_CORE_ASS(false, "WHAT TYPE OF SORCERY SHADER IS THIS CRAP!???");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath) : m_FilePath(filePath)
	{
		JB_PROFILE_FUNCTION();
		Utils::CreateCacheDirectoryIfNeeded();
		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);

		JB_CORE_WAR(filePath);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			JB_CORE_WAR("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// ass/shader/texture.glsl
		std::filesystem::path path = filePath;
		m_Name = path.stem().string();
		JB_CORE_IFO("PATH : {0}", path.string());
		JB_CORE_IFO("NAME : {0}", m_Name);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertSource, const std::string& fragSource) : m_Name(name)
	{
		JB_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertSource;
		sources[GL_FRAGMENT_SHADER] = fragSource;

		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
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
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				JB_CORE_ERR("COULD NOT READ UR SHITTY ASS FILE '{0}'", filePath);
			}
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
			JB_CORE_ASS(Utils::ShaderTypeFromString(type), "WHAT DRUG WERE YOU ON, TRYING TO USE AN INVALID TYPE SPECIFIER???");
			
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			JB_CORE_ASS(nextLinePos != std::string::npos, "YET ANOTHER FUCKED SYNTAX ERROR!!!");
			pos = source.find(typeToken, nextLinePos);
			sources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		return sources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					JB_CORE_ERR(module.GetErrorMessage());
					JB_CORE_ASS(false);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		shaderData.clear();
		m_OpenGLSourceCode.clear();
		
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));
			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
			
				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];
			
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					JB_CORE_ERR("FUCKBOY COMPILER ERROR : {0}", module.GetErrorMessage());
					JB_CORE_ASS(false);
				}
			
				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
			
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram(); std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}
		glLinkProgram(program);
		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			JB_CORE_ERR("HIEROGLYPHIC LINKER ERROR HAHAH!! ({0}):\n{1}", m_FilePath, infoLog.data());
			glDeleteProgram(program);
			for (auto id : shaderIDs)
				glDeleteShader(id);
		}
		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
		m_RendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		JB_CORE_TRC("OpenGLShader::Reflect : {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		JB_CORE_TRC("    {0} uniform buffers", resources.uniform_buffers.size());
		JB_CORE_TRC("    {0} resources", resources.sampled_images.size());

		JB_CORE_TRC("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			JB_CORE_TRC("  {0}", resource.name);
			JB_CORE_TRC("    Size : {0}", bufferSize);
			JB_CORE_TRC("    Binding : {0}", binding);
			JB_CORE_TRC("    Members : {0}", memberCount);
		}
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