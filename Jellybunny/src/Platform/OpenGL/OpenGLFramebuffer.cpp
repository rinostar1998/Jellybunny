#include "jbpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Jellybunny
{
	static const uint32_t s_MaxFramebufferSize = 8192;
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisample, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisample), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static GLenum JellybunnyTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:		return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			JB_CORE_ASS(false, "Ya really turned into a Jared to fuck it up this badly...");
			return 0;
		}

	}

	OpenGlFramebuffer::OpenGlFramebuffer(const FramebufferSpecs spec) : m_spec(spec)
	{
		for (auto spec : m_spec.Attachments.attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat)) m_ColorAttachmentSpecs.emplace_back(spec);
			else m_DepthAttachmentSpec = spec;
		}
		Invalidate();
	}

	OpenGlFramebuffer::~OpenGlFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGlFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_spec.Samples > 1;

		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
			// Attachments
			for (size_t i = 0; i < m_ColorAttachmentSpecs.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecs[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:		Utils::AttachColorTexture(m_ColorAttachments[i], m_spec.Samples, GL_RGBA8, GL_RGBA, m_spec.Width, m_spec.Height, i); break;
					case FramebufferTextureFormat::RED_INTEGER: Utils::AttachColorTexture(m_ColorAttachments[i], m_spec.Samples, GL_R32I,  GL_RED_INTEGER, m_spec.Width, m_spec.Height, i); break;
				}
			}
		}

		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpec.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_spec.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_spec.Width, m_spec.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			JB_CORE_ASS(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		JB_CORE_ASS(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "YA FORGOT TO FINISH THE TEXTURE!!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGlFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_spec.Width, m_spec.Height);
	}

	void OpenGlFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGlFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if(width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			JB_CORE_WAR("YA TRIED TO RESIZE THE FRAMEBUFFER SOME INSANE NUMBER, LIKE ({0}, {1})", width, height);
			return;
		}
		m_spec.Width = width;
		m_spec.Height = height;
		Invalidate();
	}

	int OpenGlFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		JB_CORE_ASS(attachmentIndex < m_ColorAttachments.size(), "Ya really done it now...");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGlFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		JB_CORE_ASS(attachmentIndex < m_ColorAttachments.size(), "Ya really fucked it up this time, JARED!!!");
		auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
		spec.TextureFormat;
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::JellybunnyTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
	}
}