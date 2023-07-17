#include "kpch.h"
#include "Framebuffer.h"

#include "glad/glad.h"

namespace kuai {
	Framebuffer::Framebuffer(uint32_t width, uint32_t height, uint32_t samples, uint32_t attachments)
	{
		props.width = width;
		props.height = height;
		props.samples = samples;
		props.attachments = attachments;

		reset();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &frameBufId);
		glDeleteTextures(colAttachments.size(), colAttachments.data());
		glDeleteTextures(1, &depthAttachment);
	}

	const uint32_t Framebuffer::getDepthAttachment()
	{
		return depthAttachment;
	}

	const std::vector<uint32_t>& Framebuffer::getColAttachments()
	{
		return colAttachments;
	}

	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufId);
		glBindTexture(getTextureTarget(props.samples > 1), depthAttachment);
		glViewport(0, 0, props.width, props.height);
	}

	void Framebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			KU_CORE_WARN("Attempted to rezize framebuffer to {0}x{1}", width, height);
			return;
		}

		props.width = width;
		props.height = height;

		reset();
	}

	void Framebuffer::reset()
	{
		if (frameBufId)
		{
			glDeleteFramebuffers(1, &frameBufId);
			glDeleteTextures(colAttachments.size(), colAttachments.data());
			glDeleteTextures(1, &depthAttachment);

			colAttachments.clear();
			depthAttachment = 0;
		}

		// Create framebuffer
		glGenFramebuffers(1, &frameBufId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufId);

		bool multisampling = props.samples > 1;

		// Add colour attachments
		if (props.attachments)
		{
			colAttachments.resize(props.attachments);
			glGenTextures(props.attachments, colAttachments.data());

			for (size_t i = 0; i < colAttachments.size(); i++)
			{
				glBindTexture(getTextureTarget(multisampling), colAttachments[i]);

				attachColTexture(i);
			}
		}
		else
		{
			// Depth-only framebuffer so specify no drawing (or reading)
			glDrawBuffer(GL_NONE);
			// glReadBuffer(GL_NONE);
		}

		// Add depth attachment
		glGenTextures(1, &depthAttachment);
		glBindTexture(getTextureTarget(multisampling), depthAttachment);
		attachDepthTexture();

		KU_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::attachColTexture(uint32_t index)
	{
		bool multisampling = props.samples > 1;
		if (multisampling)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, props.samples, GL_RGB, props.width, props.height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, props.width, props.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, getTextureTarget(multisampling), colAttachments[index], 0);
	}

	void Framebuffer::attachDepthTexture()
	{
		bool multisampling = props.samples > 1;
		if (multisampling)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, props.samples, GL_DEPTH_ATTACHMENT, props.width, props.height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, props.width, props.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			// Add border so everything sampled outside of depth map range is set to 1.0
			float borderCol[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderCol);  
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, getTextureTarget(multisampling), depthAttachment, 0);
	}

	unsigned int Framebuffer::getTextureTarget(bool multisampling)
	{
		return multisampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}
}