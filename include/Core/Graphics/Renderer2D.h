#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

struct Renderer2DStats
{
	uint32_t drawCalls;
	uint32_t numQuads;
	uint32_t numCircles;
};

class Renderer2D
{
	struct QuadVertex
	{
		glm::vec3 position;
		float textureId;
		glm::vec2 uv;
		glm::vec4 color;
	};

	struct CircleVertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		float thickness;
		glm::vec4 color;
	};

	struct TextCharVertex
	{
		glm::vec3 position;
		glm::vec2 textureUv;
		glm::vec4 color;
	};

public:
	static Renderer2D& Get();

	const Renderer2DStats& GetStats() const { return m_stats; }
	void ResetStats();
	
	void SetViewportAspectRatio(int width, int height, float aspectRatio);
	
	void BeginBatch(const OrthographicCamera& camera);
	void BeginBatch(const Camera& camera);
	void Flush();

	void DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
	void DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color = glm::vec4(1.0f));
	void DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
	void DrawQuad(const std::shared_ptr<Texture>& texture, const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color = glm::vec4(1.0f));
	void DrawQuad(const std::shared_ptr<Texture>& texture, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

	void DrawCircle(const glm::vec2& position, float radius, float thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
	void DrawCircle(const glm::vec3& position, float radius, float thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
	void DrawCircle(const glm::mat4& transform, float thickness = 1.0f, const glm::vec4& color = glm::vec4(1.0f));

private:
	Renderer2D();
	Renderer2D(const Renderer2D&) = delete; // delete copy ctor
	~Renderer2D();

	void FlushQuads();
	void FlushCircles();

private:
	/* PROPERTIES */

	glm::mat4 m_viewProjection;
	Renderer2DStats m_stats;

	/* QUADS */

	std::shared_ptr<VertexBuffer> m_quadsVB;
	std::shared_ptr<IndexBuffer> m_quadsIB;
	std::shared_ptr<VertexArray> m_quadsVA;
	QuadVertex* m_quadsVD;

	int m_textureSlots;
	uint32_t m_texturesCount;
	int m_samplers[32];
	std::shared_ptr<Texture> m_usedTextures[32];
	uint32_t m_quadsCount;

	std::shared_ptr<Shader> m_quadsShader;

	/* CIRCLES */

	std::shared_ptr<VertexBuffer> m_circlesVB;
	std::shared_ptr<IndexBuffer> m_circlesIB;
	std::shared_ptr<VertexArray> m_circlesVA;
	CircleVertex* m_circlesVD;

	uint32_t m_circlesCount;

	std::shared_ptr<Shader> m_circlesShader;
};
