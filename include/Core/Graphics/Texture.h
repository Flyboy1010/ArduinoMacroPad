#pragma once

#include <string>
#include <glm/glm.hpp>
#include <cstdint>

class Texture
{
public:
	Texture(uint32_t width, uint32_t height);
	Texture(const std::string& path, bool keepData = false);
	~Texture();

	const std::string& GetPath() const { return m_path; }
	uint32_t GetID() const { return m_id; }
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	uint32_t GetBpp() const { return m_bpp; }
	const uint32_t* GetData() const { return m_data; }
	glm::vec2 GetSize() const { return glm::vec2((float)m_width, (float)m_height); }

	bool IsSmooth() const { return m_isSmooth; }
	bool IsRepeated() const { return m_isRepeated; }
	bool HasMipmaps() const { return m_hasMipmaps; }

	bool Create(uint32_t width, uint32_t height);
	bool Load(const std::string& path, bool keepData = false);

	void SetSmooth(bool smooth);
	void SetRepeated(bool repeated);
	void GenerateMipmaps();

	void SetData(const void* data);

	void Bind(uint32_t slot = 0) const;
	void BindAsImage(uint32_t slot = 0) const;

private:
	std::string m_path;
	uint32_t m_id;
	uint32_t m_width, m_height;
	uint32_t m_bpp;
	uint32_t* m_data; // data as rgba8 format

	bool m_isSmooth;
	bool m_isRepeated;
	bool m_hasMipmaps;
};