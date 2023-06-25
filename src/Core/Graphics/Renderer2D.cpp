#include "Core/Graphics/Renderer2D.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Graphics/RenderCommand.h"

#define MAX_QUADS 10000
#define MAX_CIRCLES 10000

Renderer2D::Renderer2D()
{
	/* INIT */

	m_quadsCount = 0;
	m_circlesCount = 0;
	m_texturesCount = 0;
	m_stats = {};

	// view projection

	m_viewProjection = glm::mat4(1.0f);

	// get texture slots

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_textureSlots);

	for (int i = 0; i < m_textureSlots; i++)
	{
		m_samplers[i] = i;
		m_usedTextures[i] = nullptr;
	}
 
	/* QUADS */

	// vertex buffer

	m_quadsVB = std::make_shared<VertexBuffer>(nullptr, 4 * MAX_QUADS * sizeof(QuadVertex));

	// vertex data

	m_quadsVD = new QuadVertex[4 * MAX_QUADS];

	// vertex buffer layout

	VertexBufferLayout quadsVBL = {
		{ "position"  , ShaderDataType::Float3 },
		{ "texture_id", ShaderDataType::Float  },
		{ "uv"        , ShaderDataType::Float2 },
		{ "color"     , ShaderDataType::Float4 }
	};

	// set vertex buffer layout

	m_quadsVB->SetLayout(quadsVBL);

	// index buffer

	// generate and set index buffer data

	uint32_t* quadsIBD = new uint32_t[6 * MAX_QUADS];

	int n = 0;

	for (int i = 0; i < 6 * MAX_QUADS; i += 6) {
		quadsIBD[i] = n;
		quadsIBD[i + 1] = n + 1;
		quadsIBD[i + 2] = n + 2;
		quadsIBD[i + 3] = n + 2;
		quadsIBD[i + 4] = n + 3;
		quadsIBD[i + 5] = n;

		n += 4;
	}

	m_quadsIB = std::make_shared<IndexBuffer>(6 * MAX_QUADS, quadsIBD);

	delete[] quadsIBD;

	// vertex array

	m_quadsVA = std::make_shared<VertexArray>(m_quadsVB);

	// shader

	m_quadsShader = std::make_shared<Shader>("assets/shaders/quads.glsl");

	/* CIRCLES */

	// vertex buffer

	m_circlesVB = std::make_shared<VertexBuffer>(nullptr, 4 * MAX_CIRCLES * sizeof(CircleVertex));

	// vertex data

	m_circlesVD = new CircleVertex[4 * MAX_CIRCLES];

	// vertex buffer layout

	VertexBufferLayout circlesVBL = {
		{ "position"  , ShaderDataType::Float3 },
		{ "uv"        , ShaderDataType::Float2 },
		{ "thickness" , ShaderDataType::Float  },
		{ "color"     , ShaderDataType::Float4 }
	};

	// set vertex buffer layout

	m_circlesVB->SetLayout(circlesVBL);

	// index buffer

	uint32_t* circlesIBD = new uint32_t[6 * MAX_CIRCLES];

	int c = 0;

	for (int i = 0; i < 6 * MAX_CIRCLES; i += 6) {
		circlesIBD[i] = c;
		circlesIBD[i + 1] = c + 1;
		circlesIBD[i + 2] = c + 2;
		circlesIBD[i + 3] = c + 2;
		circlesIBD[i + 4] = c + 3;
		circlesIBD[i + 5] = c;

		c += 4;
	}

	m_circlesIB = std::make_shared<IndexBuffer>(6 * MAX_CIRCLES, circlesIBD);

	delete[] circlesIBD;

	// vertex array

	m_circlesVA = std::make_shared<VertexArray>(m_circlesVB);

	// shader

	m_circlesShader = std::make_shared<Shader>("assets/shaders/circles.glsl");

	/* OPENGL PARAMETERS TWEAKING */

	// blending

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth testing

	glEnable(GL_DEPTH_TEST);
}

Renderer2D::~Renderer2D()
{
	delete[] m_quadsVD;
	delete[] m_circlesVD;
}

Renderer2D& Renderer2D::Get()
{
	static Renderer2D instance;
	return instance;
}

void Renderer2D::ResetStats()
{
	m_stats.drawCalls = 0;
	m_stats.numQuads = 0;
	m_stats.numCircles = 0;
}

void Renderer2D::SetViewportAspectRatio(int width, int height, float aspectRatio)
{
	// get the current aspect ratio

	float currentAspectRatio = width / (float)height;

	// target viewport values to achieve that ar

	float targetX = 0;
	float targetY = 0;
	float targetWidth = (float)width;
	float targetHeight = (float)height;

	if (currentAspectRatio > aspectRatio)
	{
		targetWidth = height * aspectRatio;
		targetX = (width - targetWidth) / 2.0f;
	}
	else if (currentAspectRatio < aspectRatio)
	{
		targetHeight = width * (1.0f / aspectRatio);
		targetY = (height - targetHeight) / 2.0f;
	}

	// set the viewport

	RenderCommand::SetViewport(targetX, targetY, targetWidth, targetHeight);
}

void Renderer2D::BeginBatch(const OrthographicCamera& camera)
{
	// view projection matrix

	m_viewProjection = camera.GetProjection() * camera.GetTransform();

	// reset for quads

	m_quadsCount = 0;
	m_texturesCount = 0;

	// reset for circles

	m_circlesCount = 0;
}

void Renderer2D::BeginBatch(const Camera& camera)
{
	// view projection matrix

	m_viewProjection = camera.GetProjection() * camera.GetTransform();

	// reset for quads

	m_quadsCount = 0;
	m_texturesCount = 0;

	// reset for circles

	m_circlesCount = 0;
}

void Renderer2D::FlushQuads()
{
	if (m_quadsCount > 0)
	{
		// bind shader

		m_quadsShader->Bind();
		m_quadsShader->SetMat4("u_viewProjection", m_viewProjection);
		m_quadsShader->SetIntArray("u_textures", m_textureSlots, m_samplers);
		
		// bind textures used

		for (uint32_t i = 0; i < m_texturesCount; i++)
		{
			m_usedTextures[i]->Bind(i);
			m_usedTextures[i] = nullptr; // reset
		}

		// bind quads_vbo and set data

		m_quadsVB->Bind();
		m_quadsVB->SetData(m_quadsVD, 4 * (size_t)m_quadsCount * sizeof(QuadVertex));

		// draw call

		RenderCommand::DrawTriangles(m_quadsVA, m_quadsIB, 6 * m_quadsCount);

		// stats

		m_stats.drawCalls++;
		m_stats.numQuads += m_quadsCount;

		// reset

		m_quadsCount = 0;
		m_texturesCount = 0;
	}
}

void Renderer2D::FlushCircles()
{
	if (m_circlesCount > 0)
	{
		// bind shader

		m_circlesShader->Bind();
		m_circlesShader->SetMat4("u_viewProjection", m_viewProjection);

		// bind circles vb and set data

		m_circlesVB->Bind();
		m_circlesVB->SetData(m_circlesVD, 4 * (size_t)m_circlesCount * sizeof(CircleVertex));

		// draw call

		RenderCommand::DrawTriangles(m_circlesVA, m_circlesIB, 6 * m_circlesCount);

		// stats

		m_stats.drawCalls++;
		m_stats.numCircles += m_circlesCount;

		// reset

		m_circlesCount = 0;
	}
}

void Renderer2D::Flush()
{
	// quads

	FlushQuads();

	// circles

	FlushCircles();
}

void Renderer2D::DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad(texture, glm::vec3(position, 0.0f), size, color);
}

void Renderer2D::DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawQuad(texture, glm::vec3(position, 0.0f), size, rotation, color);
}

void Renderer2D::DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

	DrawQuad(texture, transform, color);
}

void Renderer2D::DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) 
		* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

	DrawQuad(texture, transform, color);
}

void Renderer2D::DrawQuad(const std::shared_ptr<Texture>& texture, const glm::mat4& transform, const glm::vec4& color)
{
	if (texture == nullptr)
		return;

	// check if it needs to make a new batch

	if (m_quadsCount >= MAX_QUADS || m_texturesCount >= m_textureSlots)
		FlushQuads();

	// get texture slot

	int slot = -1;

	for (int i = 0; i < m_texturesCount; i++)
	{
		if (texture == m_usedTextures[i])
		{
			slot = i;
			break;
		}
	}

	if (slot == -1)
	{
		m_usedTextures[m_texturesCount] = texture;
		slot = m_texturesCount;
		m_texturesCount++;
	}

	// normalized quad

	static const glm::vec4 normalizedQuad[] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	// uvs

	static const glm::vec2 uvs[] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	// batch

	int i = m_quadsCount * 4;

	for (int j = 0; j < 4; j++)
	{
		QuadVertex& qv = m_quadsVD[i + j];

		qv.position = transform * normalizedQuad[j];
		qv.textureId = slot;
		qv.uv = uvs[j];
		qv.color = color;
	}

	m_quadsCount++;
}

void Renderer2D::DrawCircle(const glm::vec2& position, float radius, float thickness, const glm::vec4& color)
{
	DrawCircle(glm::vec3(position, 0.0f), radius, thickness, color);
}

void Renderer2D::DrawCircle(const glm::vec3& position, float radius, float thickness, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { radius, radius, 1.0f });

	DrawCircle(transform, thickness, color);
}

void Renderer2D::DrawCircle(const glm::mat4& transform, float thickness, const glm::vec4& color)
{
	if (m_circlesCount >= MAX_CIRCLES)
		FlushCircles();

	// normalized quad

	static const glm::vec4 normalizedQuad[] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	// uvs

	static const glm::vec2 uvs[] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	// batch

	int i = m_circlesCount * 4;

	for (int j = 0; j < 4; j++)
	{
		CircleVertex& cv = m_circlesVD[i + j];

		cv.position = transform * normalizedQuad[j];
		cv.uv = uvs[j];
		cv.thickness = thickness;
		cv.color = color;
	}

	m_circlesCount++;
}