#include "Core/Graphics/Framebuffer.h"
#include <GL/glew.h>

static void CreateColorAttachment(unsigned char id, unsigned int width, unsigned int height, GLint format, unsigned int* colorAttachment)
{
    glGenTextures(1, colorAttachment);
    glBindTexture(GL_TEXTURE_2D, *colorAttachment);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // attach the color buffer to the framebuffer

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + id, GL_TEXTURE_2D, *colorAttachment, 0);
}

static void CreateDepthAttachment(unsigned int width, unsigned int height, GLint format, GLenum attachmentType, unsigned int* depthAttachment)
{
    glGenTextures(1, depthAttachment);
    glBindTexture(GL_TEXTURE_2D, *depthAttachment);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // attach the depth buffer to the framebuffer

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, *depthAttachment, 0);
}

/* FRAMEBUFFER */

Framebuffer::Framebuffer()
{
    m_id = 0;
    m_depthAttachment = 0;
    m_specification.width = 0;
    m_specification.height = 0;
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
{
    m_id = other.m_id;
    m_depthAttachment = other.m_depthAttachment;
    m_specification = std::move(other.m_specification);
    m_colorAttachmentSpecifications = std::move(other.m_colorAttachmentSpecifications);
    m_depthAttachmentSpecification = other.m_depthAttachmentSpecification;

    other.m_id = 0;
    other.m_depthAttachment = 0;
    other.m_depthAttachmentSpecification = {};
}

Framebuffer::Framebuffer(const FramebufferSpecification& specification)
{
    m_id = 0;
    m_depthAttachment = 0;
    m_specification.width = 0;
    m_specification.height = 0;

    Create(specification);
}

Framebuffer::~Framebuffer()
{
    Destroy();
}

unsigned int Framebuffer::GetColorAttachmentId(int index) const
{
    if (index >= 0 && index < m_colorAttachments.size())
        return m_colorAttachments[index];

    return 0;
}

void Framebuffer::Destroy()
{
    // destroy fbo

    glDeleteFramebuffers(1, &m_id);

    // destroy color attachments

    glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
    
    // destroy depth attachment

    glDeleteTextures(1, &m_depthAttachment);
}

bool Framebuffer::Create(const FramebufferSpecification& specification)
{
    // destroy to avoid memory leaks during re-creation

    Destroy();
    m_colorAttachmentSpecifications.clear();
    m_colorAttachments.clear();
    m_depthAttachmentSpecification = {};
    m_depthAttachment = 0;

    // check specification attachments type

    m_specification = specification;

    const FramebufferAttachmentSpecification& attachmentSpecification = specification.attachmentSpecification;

    for (const auto& attachment : attachmentSpecification.attachments)
    {
        switch (attachment.textureFormat)
        {
        case FramebufferTextureFormat::Depth:
            m_depthAttachmentSpecification = attachment;
            break;
        default:
            m_colorAttachmentSpecifications.push_back(attachment);
        }
    }

    // create and bind the framebuffer

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // create and attach color attachments

    if (m_colorAttachmentSpecifications.size() > 0)
    {
        // resize the vector with the ids of the color attachments

        m_colorAttachments.resize(m_colorAttachmentSpecifications.size());

        // for each color attachment

        for (int i = 0; i < m_colorAttachments.size(); i++)
        {
            // check the texture format

            switch (m_colorAttachmentSpecifications[i].textureFormat)
            {
            case FramebufferTextureFormat::RGBA32F:
                CreateColorAttachment(i, specification.width, specification.height, GL_RGBA32F, &m_colorAttachments[i]);
                break;
            case FramebufferTextureFormat::RGBA8:
                CreateColorAttachment(i, specification.width, specification.height, GL_RGBA8, &m_colorAttachments[i]);
                break;
            default:
                CreateColorAttachment(i, specification.width, specification.height, GL_RGBA8, &m_colorAttachments[i]);
            }
        }
    }

    // create and attach depth attachment

    if (m_depthAttachmentSpecification.textureFormat != FramebufferTextureFormat::None)
    {
        switch (m_depthAttachmentSpecification.textureFormat)
        {
        case FramebufferTextureFormat::Depth:
            CreateDepthAttachment(specification.width, specification.height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, &m_depthAttachment);
            break;
        default:
            CreateDepthAttachment(specification.width, specification.height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, &m_depthAttachment);
        }
    }

    // check if there are more than 1 color attachment

    if (m_colorAttachments.size() > 1)
    {
        // fixed array with possible color buffers

        GLenum colorBuffers[16];

        for (int i = 0; i < 16; i++)
            colorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

        // if there are more than 1 color buffer we need to tell explicitly to opengl to draw to those buffers aswell

        glDrawBuffers(m_colorAttachments.size(), colorBuffers);
    }
    else if (m_colorAttachments.size() == 0) // check if there are no color attachments at all
    {
        // only depth pass

        glDrawBuffer(GL_NONE);
    }

    // unbind the Framebuffer

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // check if succesfully created

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::Bind(const Framebuffer* framebuffer)
{
    if (framebuffer != nullptr)
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_id);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
    if (this != &other)
    {
        Destroy(); // first destroy the attachments and the fb

        m_id = other.m_id;
        m_depthAttachment = other.m_depthAttachment;
        m_specification = std::move(other.m_specification);
        m_colorAttachmentSpecifications = std::move(other.m_colorAttachmentSpecifications);
        m_depthAttachmentSpecification = other.m_depthAttachmentSpecification;

        other.m_id = 0;
        other.m_depthAttachment = 0;
        other.m_depthAttachmentSpecification = {};
    }

    return *this;
}
