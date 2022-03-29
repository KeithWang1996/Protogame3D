#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

GPUMesh::GPUMesh(RenderContext* owner)
	: m_owner(owner)
{
	m_vertices = new VertexBuffer(owner, MEMORY_HINT_DYNAMIC);
	m_indices = new IndexBuffer(owner, MEMORY_HINT_DYNAMIC);
}
GPUMesh::~GPUMesh()
{
	delete m_vertices;
	delete m_indices;
	m_vertices = nullptr;
	m_indices = nullptr;
}
void GPUMesh::UpdateVertices(unsigned int vcount, void const* vertexData, unsigned int vertexStride, buffer_attribute_t const* layout)
{
	size_t bufferTotalByteSize = vcount * vertexStride;
	size_t elementSize = vertexStride;
	m_vertices->Update(vertexData, bufferTotalByteSize, elementSize);
	m_vertices->m_layout = layout;
	m_vertexCount = vcount;
}

void GPUMesh::UpdateIndices(unsigned int icount, unsigned int const* indices)
{
	size_t bufferTotalByteSize = icount * sizeof(unsigned int);
	size_t elementSize = sizeof(unsigned int);
	if (icount > 0)
	{
		m_indices->Update(indices, bufferTotalByteSize, elementSize);
	}
	m_indexCount = icount;
}