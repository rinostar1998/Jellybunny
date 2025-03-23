#include "jbpch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"

namespace Jellybunny
{
	OpenGLTexture2D::OpenGLTexture2D(const uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
	{
		JB_PROFILE_FUNCTION();
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath) : m_FilePath(filePath)
	{
		JB_PROFILE_FUNCTION();
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			JB_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath) : stbi_load");
			data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		}
		JB_CORE_ASS(data, "UR SHITTY ASS DICK PIC FAILED TO IMPORT!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if		(channels == 4) { internalFormat = GL_RGBA8; dataFormat = GL_RGBA; }
		else if (channels == 3) { internalFormat = GL_RGB8;  dataFormat = GL_RGB; }

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		JB_CORE_ASS(internalFormat & dataFormat, "UR CRAPPY TEXTURE FORMAT DOES NOT WORK HERE!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}


	OpenGLTexture2D::~OpenGLTexture2D()
	{
		JB_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		JB_PROFILE_FUNCTION();
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		JB_CORE_ASS(size == m_Width * m_Height * bpp, "WTF IS THIS!??? YA DIDN'T GIVE ME THE CORRECT SIZE OF TEXTURE DATA FOR THIS, DUMBASS!!!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		JB_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_RendererID);
	}
}