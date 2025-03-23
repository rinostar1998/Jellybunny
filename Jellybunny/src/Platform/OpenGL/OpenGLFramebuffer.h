#pragma once
#include "Jellybunny/Renderer/Framebuffer.h"

namespace Jellybunny
{
	class OpenGlFramebuffer : public Framebuffer
	{
	public:
		OpenGlFramebuffer(const FramebufferSpecs spec);
		virtual ~OpenGlFramebuffer();

		void Invalidate();
		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { JB_CORE_ASS(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecs& GetSpec() const override { return m_spec; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecs m_spec;

		std::vector<FramebufferTextureSpecs> m_ColorAttachmentSpecs;
		FramebufferTextureSpecs m_DepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment;
	};
}