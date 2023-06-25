#include "Core/Graphics/ComputeShader.h"
#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <fstream>
 
/* parse the shader */

static std::string ParseComputeShader(const std::string& path)
{
	std::ifstream file(path);
	std::stringstream code;

	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			code << line << '\n';
		}
	}

	return code.str();
}

/* compile shader */

static uint32_t CompileComputeShader(const std::string& code)
{
	uint32_t computeShaderId = glCreateShader(GL_COMPUTE_SHADER);

	const char* cstrCode = code.c_str();

	glShaderSource(computeShaderId, 1, &cstrCode, nullptr);
	glCompileShader(computeShaderId);

	int compileStatus;
	glGetShaderiv(computeShaderId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		int length;
		glGetShaderiv(computeShaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(computeShaderId, length, &length, message);

		std::cout << message << std::endl;

		delete[] message;

		glDeleteShader(computeShaderId);

		return 0;
	}

	return computeShaderId;
}

/* COMPUTE SHADER CLASS */

ComputeShader::ComputeShader()
{
	m_id = 0;
}

ComputeShader::ComputeShader(ComputeShader&& other) noexcept
{
	m_id = other.m_id;
	m_path = std::move(other.m_path);
	m_uniformCache = std::move(other.m_uniformCache);

	other.m_id = 0;
}

ComputeShader::ComputeShader(const std::string& path)
{
	Load(path);
}

ComputeShader::~ComputeShader()
{
	glDeleteProgram(m_id);

	std::cout << "[INFO] ComputeShader destroyed \"" << m_path << "\"" << std::endl;
}

bool ComputeShader::Load(const std::string& path)
{
	// parse the compute shader

	std::string computeShaderCode = ParseComputeShader(path);

	if (computeShaderCode.empty())
		return false;

	// compile the compute shader

	uint32_t computeShaderId = CompileComputeShader(computeShaderCode);

	if (computeShaderId == 0)
		return false;

	// if all correct then create the shader program and attach the shaders

	m_id = glCreateProgram();
	m_path = path;

	glAttachShader(m_id, computeShaderId);
	glLinkProgram(m_id);
	glValidateProgram(m_id);

	glDeleteShader(computeShaderId);

	// shader corretly

	std::cout << "[INFO] ComputeShader loaded \"" << path << "\"" << std::endl;

	return true;
}

void ComputeShader::Dispatch(uint32_t nGroupsX, uint32_t nGroupsY, uint32_t nGroupsZ) const
{
	glUseProgram(m_id);
	glDispatchCompute(nGroupsX, nGroupsY, nGroupsZ);
	glMemoryBarrier(GL_ALL_BARRIER_BITS); // TODO: find info about what this does
}

int ComputeShader::GetUniformLocation(const std::string& name) const
{
	// check if the uniform is already in the "cache"

	auto it = m_uniformCache.find(name);

	if (it != m_uniformCache.end())
		return it->second;

	// if not get the location

	int location = glGetUniformLocation(m_id, name.c_str());

	// check if doesnt exist to output a warning

	if (location == -1)
		std::cout << "[WARNING] Uniform \"" << name << "\" does not exist" << std::endl;

	// even if it does not exist store the uniform into the hashmap

	m_uniformCache[name] = location;

	return location;
}

void ComputeShader::SetInt(const std::string& name, int data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1i(uniformId, data);
}

void ComputeShader::SetFloat(const std::string& name, float data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1f(uniformId, data);
}

void ComputeShader::SetIntArray(const std::string& name, int count, const int* data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1iv(uniformId, count, data);
}

void ComputeShader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniformMatrix4fv(uniformId, 1, false, &mat[0][0]);
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform2f(uniformId, data.x, data.y);
}

void ComputeShader::SetVec3(const std::string& name, const glm::vec3& data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform3f(uniformId, data.x, data.y, data.z);
}

void ComputeShader::SetVec4(const std::string& name, const glm::vec4& data) const
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform4f(uniformId, data.x, data.y, data.z, data.w);
}

void ComputeShader::Bind(const ComputeShader* computeShader)
{
	if (computeShader != nullptr)
		glUseProgram(computeShader->m_id);
	else
		glUseProgram(0);
}

ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
{
	if (this != &other)
	{
		m_id = other.m_id;
		m_path = std::move(other.m_path);
		m_uniformCache = std::move(other.m_uniformCache);

		other.m_id = 0;
	}

	return *this;
}
