#pragma once
#include "Jellybunny/Renderer/Texture.h"
#include <glad/glad.h>


namespace Jellybunny
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& filePath);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual const std::string& GetPath() const override { return m_FilePath; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot) const override;
		virtual bool operator==(const Texture& other) const override { return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; }
	private:
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}