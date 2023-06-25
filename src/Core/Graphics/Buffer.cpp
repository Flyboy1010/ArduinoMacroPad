#include "Core/Graphics/Buffer.h"
#include <GL/glew.h>

// get the size of a data type in bytes

static size_t ShaderDataTypeToSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat3:     return 4 * 3 * 3;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Int:      return 4;
	case ShaderDataType::Int2:     return 4 * 2;
	case ShaderDataType::Int3:     return 4 * 3;
	case ShaderDataType::Int4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	return 0;
}

static uint32_t ShaderDataTypeToCount(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;
	case ShaderDataType::Mat3:    return 9;
	case ShaderDataType::Mat4:    return 16;
	case ShaderDataType::Int:     return 1;
	case ShaderDataType::Int2:    return 2;
	case ShaderDataType::Int3:    return 3;
	case ShaderDataType::Int4:    return 4;
	case ShaderDataType::Bool:    return 1;
	}

	return 0;
}

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return GL_FLOAT;
	case ShaderDataType::Float2:   return GL_FLOAT;
	case ShaderDataType::Float3:   return GL_FLOAT;
	case ShaderDataType::Float4:   return GL_FLOAT;
	case ShaderDataType::Mat3:     return GL_FLOAT;
	case ShaderDataType::Mat4:     return GL_FLOAT;
	case ShaderDataType::Int:      return GL_INT;
	case ShaderDataType::Int2:     return GL_INT;
	case ShaderDataType::Int3:     return GL_INT;
	case ShaderDataType::Int4:     return GL_INT;
	case ShaderDataType::Bool:     return GL_BOOL;
	}

	return 0;
}

/* VERTEX BUFFER LAYOUT */

VertexBufferLayout::VertexBufferLayout()
{
	m_stride = 0;
}

VertexBufferLayout::VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
{
	m_stride = 0;

	for (const auto& element : elements)
	{
		m_stride += ShaderDataTypeToSize(element.type);
	}

	m_elements.insert(m_elements.end(), elements.begin(), elements.end());
}

/* VERTEX BUFFER */

VertexBuffer::VertexBuffer(const void* data, size_t size)
{
	// create the buffer

	m_size = size;

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, data == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void VertexBuffer::SetData(const void* data, size_t size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_layout = layout;
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

/* INDEX BUFFER */

IndexBuffer::IndexBuffer(uint32_t count, const uint32_t* data)
{
	// create the buffer

	m_count = count;
	m_size = count * sizeof(uint32_t);

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

/* VERTEX ARRAY */

VertexArray::VertexArray(const std::shared_ptr<VertexBuffer>& vb)
{
	// create and bind vertex array

	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);

	// bind vertex buffer

	vb->Bind();

	// set layout

	const VertexBufferLayout& layout = vb->GetLayout();

	const auto& elements = layout.GetElements();
	size_t stride = layout.GetStride();
	size_t offset = 0;

	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& e = elements[i];

		uint32_t count = ShaderDataTypeToCount(e.type); // in number of components
		size_t size = ShaderDataTypeToSize(e.type); // total size
		GLenum typeGL = ShaderDataTypeToOpenGLBaseType(e.type);

		/*switch (e.type)
		{
		case GL_FLOAT:
			glVertexAttribPointer(i, e.count, e.type, GL_FALSE, stride, (const void*)offset);
			break;
		case GL_INT:
		case GL_UNSIGNED_INT:
			glVertexAttribIPointer(i, e.count, e.type, stride, (const void*)offset);
			break;
		}*/

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, count, typeGL, GL_FALSE, stride, (const void*)offset);

		offset += size;
	}
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_id);
}

/* UNIFORM BUFFER */

UniformBuffer::UniformBuffer(const void* data, size_t size)
{
	m_size = size;

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW); // STATIC OR DYNAMIC
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void UniformBuffer::SetData(const void* data, size_t offset, size_t size)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UniformBuffer::Bind(uint32_t binding) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
}

/* SHADER STORAGE BUFFER */

ShaderStorageBuffer::ShaderStorageBuffer(const void* data, size_t size)
{
	m_size = size;

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void ShaderStorageBuffer::SetData(const void* data, size_t offset, size_t size)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void ShaderStorageBuffer::GetData(void* data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
	const void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	memcpy(data, ptr, m_size);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void ShaderStorageBuffer::Bind(uint32_t binding) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_id);
}