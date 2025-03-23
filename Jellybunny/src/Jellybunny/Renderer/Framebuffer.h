#pragma once
#include "Jellybunny/Core/Core.h"

namespace Jellybunny
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		// color
		RED_INTEGER,
		RGBA8,
		// depth
		DEPTH24STENCIL8,
		// defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecs
	{
		FramebufferTextureSpecs() = default;
		FramebufferTextureSpecs(FramebufferTextureFormat format) : TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;

	};

	struct FrameBufferAttachmentSpecs
	{
		FrameBufferAttachmentSpecs() = default;
		FrameBufferAttachmentSpecs(std::initializer_list<FramebufferTextureSpecs> attachments) : attachments(attachments) {}

		std::vector<FramebufferTextureSpecs> attachments;
	};

	struct FramebufferSpecs
	{
		uint32_t Width, Height;
		FrameBufferAttachmentSpecs Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual const FramebufferSpecs& GetSpec() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecs& spec);
	};
}