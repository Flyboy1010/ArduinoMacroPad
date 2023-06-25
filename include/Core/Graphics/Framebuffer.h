#pragma once

#include <vector>
#include <initializer_list>
#include <cstdint>

enum class FramebufferTextureFormat
{
	None = 0,
	RGBA32F, // f32 per component
	RGBA8,   // 8 bit per component
	Depth    // depth
};

struct FramebufferTextureSpecification
{
	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat format)
		: textureFormat(format) {}

	FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None; // format
	// TODO: texture wrap
	// TODO: texture filter
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> _attachments)
		: attachments(_attachments) {}

	std::vector<FramebufferTextureSpecification> attachments;
};

struct FramebufferSpecification
{
	uint32_t width = 0, height = 0;
	FramebufferAttachmentSpecification attachmentSpecification;
};

class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer(const FramebufferSpecification& specification);
	~Framebuffer();

	uint32_t GetWidth() const { return m_specification.width; }
	uint32_t GeHeight() const { return m_specification.height; }
	uint32_t GetColorAttachmentId(int index) const;
	uint32_t GetDepthAttachmentId() const { return m_depthAttachment; }

	bool Create(const FramebufferSpecification& specification);

	// bind framebuffer

	static void Bind(const Framebuffer* framebuffer);

	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

private:
	void Destroy();

private:
	uint32_t m_id;
	FramebufferSpecification m_specification;

	std::vector<FramebufferTextureSpecification> m_colorAttachmentSpecifications;
	FramebufferTextureSpecification m_depthAttachmentSpecification;

	std::vector<uint32_t> m_colorAttachments;
	uint32_t m_depthAttachment;
};