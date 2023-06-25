#pragma once

#include <memory>
#include <vector>
#include <initializer_list>
#include <cstddef>
#include <cstdint>
#include <string>

enum class ShaderDataType
{
	None,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

struct VertexBufferElement
{
	std::string name;
	ShaderDataType type;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();
	VertexBufferLayout(std::initializer_list<VertexBufferElement> elements);

	const std::vector<VertexBufferElement>& GetElements() const { return m_elements; }
	size_t GetStride() const { return m_stride; }

private:
	std::vector<VertexBufferElement> m_elements;
	size_t m_stride;
};

class VertexBuffer
{
public:
	VertexBuffer(const void* data, size_t size);
	~VertexBuffer();

	uint32_t GetID() const { return m_id; }
	const VertexBufferLayout& GetLayout() const { return m_layout; }

	void SetData(const void* data, size_t size);
	void SetLayout(const VertexBufferLayout& layout);

	void Bind() const;

private:
	uint32_t m_id;
	size_t m_size;
	VertexBufferLayout m_layout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t count, const uint32_t* data); // data = array with indices
	~IndexBuffer();

	uint32_t GetID() const { return m_id; }
	uint32_t GetCount() const { return m_count; }

	void Bind() const;

private:
	uint32_t m_id;
	uint32_t m_count;
	size_t m_size;
};

class VertexArray
{
public:
	VertexArray(const std::shared_ptr<VertexBuffer>& vb);
	~VertexArray();

	uint32_t GetID() const { return m_id; }

	void Bind() const;

private:
	uint32_t m_id;
};

class UniformBuffer
{
public:
	UniformBuffer(const void* data, size_t size);
	~UniformBuffer();

	uint32_t GetID() const { return m_id; }
	size_t GetSize() const { return m_size; }

	void SetData(const void* data, size_t offset, size_t size);

	void Bind(uint32_t binding) const;

private:
	uint32_t m_id;
	size_t m_size;
};

class ShaderStorageBuffer
{
public:
	ShaderStorageBuffer(const void* data, size_t size);
	~ShaderStorageBuffer();

	uint32_t GetID() const { return m_id; }
	size_t GetSize() const { return m_size; }

	void SetData(const void* data, size_t offset, size_t size);
	void GetData(void* data); // retrieve the data from the gpu

	void Bind(uint32_t binding) const;

private:
	uint32_t m_id;
	size_t m_size;
};