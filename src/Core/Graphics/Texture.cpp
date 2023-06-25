#include "Core/Graphics/Texture.h"
#include <GL/glew.h>
#include <iostream>
#include <stb/stb_image.h>

Texture::Texture(uint32_t width, uint32_t height)
{
	Create(width, height);
}

Texture::Texture(const std::string& path, bool keepData)
{
	Load(path, keepData);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
	stbi_image_free(m_data);

	if (!m_path.empty())
		std::cout << "[INFO] Texture destroyed \"" << m_path << "\"" << std::endl;
	else
		std::cout << "[INFO] User created texture destroyed" << std::endl;
}

bool Texture::Create(uint32_t width, uint32_t height)
{
	// set properties

	m_width = width;
	m_height = height;
	m_bpp = 4;
	m_data = nullptr;

	m_isSmooth = false;
	m_isRepeated = false;
	m_hasMipmaps = false;

	// opengl create texture

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	// opengl texture parameters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// set data

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	return true;
}

bool Texture::Load(const std::string& path, bool keepData)
{
	// load image

	int width, height, bpp;
	stbi_set_flip_vertically_on_load(true);
	m_data = (uint32_t*)stbi_load(path.c_str(), &width, &height, &bpp, 4);

	// init

	m_path = path;
	m_width = width;
	m_height = height;
	m_bpp = bpp;
	
	m_isSmooth = false;
	m_isRepeated = false;
	m_hasMipmaps = false;

	// check if the image is loaded

	if (m_data == nullptr)
	{
		std::cout << "[ERROR] Texture loading \"" << path << "\"" << std::endl;
		return false;
	}

	// if loaded succesfully then create the texture

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	// opengl texture parameters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// set data

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);

	// keep or not the pixel data

	if (!keepData)
	{
		stbi_image_free(m_data);
		m_data = nullptr;
	}

	// log

	std::cout << "[INFO] Texture loaded \"" << path << "\"" << std::endl;

	return true;
}

void Texture::SetSmooth(bool smooth)
{
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

	if (m_hasMipmaps)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

	m_isSmooth = smooth;
}

void Texture::SetRepeated(bool repeated)
{
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	m_isRepeated = repeated;
}

void Texture::GenerateMipmaps()
{
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_hasMipmaps = true;
}

void Texture::SetData(const void* data)
{
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);

	m_hasMipmaps = false;
}

void Texture::Bind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::BindAsImage(uint32_t slot) const
{
	glBindImageTexture(slot, m_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}
