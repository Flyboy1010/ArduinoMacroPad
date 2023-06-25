#include "Core/Graphics/RenderCommand.h"
#include <GL/glew.h>

void RenderCommand::Clear(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void RenderCommand::DrawTriangles(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib, uint32_t indices)
{
	// bind vertex array

	va->Bind();

	// bind index buffer

	ib->Bind();

	// num of indices

	uint32_t numIndices = indices == 0 ? ib->GetCount() : indices;

	// draw call

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
}
