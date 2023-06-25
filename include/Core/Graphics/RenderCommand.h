#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <cstdint>
#include "Buffer.h"

class RenderCommand
{
public:
	static void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f });

	static void SetViewport(int x, int y, int width, int height);

	static void DrawTriangles(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib, uint32_t indices = 0);

private:
	RenderCommand() {}
	~RenderCommand() {}
};