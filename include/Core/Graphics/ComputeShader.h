#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "Texture.h"

class ComputeShader
{
public:
	ComputeShader();
	ComputeShader(const ComputeShader&) = delete;
	ComputeShader(ComputeShader&& other) noexcept;
	ComputeShader(const std::string& path);
	~ComputeShader();

	uint32_t GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

	bool Load(const std::string& path);

	void Dispatch(uint32_t nGroupsX, uint32_t nGroupsY, uint32_t nGroupsZ) const;

	void SetInt(const std::string& name, int data) const;
	void SetFloat(const std::string& name, float data) const;
	void SetIntArray(const std::string& name, int count, const int* data) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetVec2(const std::string& name, const glm::vec2& data) const;
	void SetVec3(const std::string& name, const glm::vec3& data) const;
	void SetVec4(const std::string& name, const glm::vec4& data) const;

	// bind

	static void Bind(const ComputeShader* computeShader);

	// operators

	ComputeShader& operator=(const ComputeShader&) = delete; // delete copy operator
	ComputeShader& operator=(ComputeShader&& other) noexcept; // move operator

private:
	int GetUniformLocation(const std::string& name) const;

private:
	uint32_t m_id;
	std::string m_path;
	mutable std::unordered_map<std::string, int> m_uniformCache;
};

