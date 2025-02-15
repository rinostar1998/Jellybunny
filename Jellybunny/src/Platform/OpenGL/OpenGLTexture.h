#pragma once
#include "Jellybunny/Renderer/Texture.h"

namespace Jellybunny
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& filePath);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot) const override;
	private:
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
}