#pragma once

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include "Texture.h"

class Shader
{
public:
	Shader();
	Shader(const Shader&) = delete; // delete copy ctor
	Shader(Shader&& other) noexcept; // move operator
	Shader(const std::string& path);
	~Shader();

	uint32_t GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

	bool Load(const std::string& path); // load

	void SetInt(const std::string& name, int data) const;
	void SetFloat(const std::string& name, float data) const;
	void SetIntArray(const std::string& name, int count, const int* data) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetVec2(const std::string& name, const glm::vec2& data) const;
	void SetVec3(const std::string& name, const glm::vec3& data) const;
	void SetVec4(const std::string& name, const glm::vec4& data) const;
	void SetTexture(const std::string& name, const Texture& texture, uint32_t textureSlot) const;

	void Bind() const;

	// operators

	Shader& operator=(const Shader&) = delete; // delete copy operator
	Shader& operator=(Shader&& other) noexcept; // move operator

private:
	int GetUniformLocation(const std::string& name) const;

private:
	uint32_t m_id;
	std::string m_path;
	mutable std::unordered_map<std::string, int> m_uniformCache; // mutable to be able to modify in the get uniform location const function
};