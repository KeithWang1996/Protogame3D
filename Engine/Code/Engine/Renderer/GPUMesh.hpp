#pragma once
#include <vector>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_Lit.hpp"
class RenderContext;
struct buffer_attribute_t;
class VertexBuffer;
class IndexBuffer;
class GPUMesh
{
public:
	GPUMesh(RenderContext* owner);
	~GPUMesh();
	void UpdateVertices(unsigned int vcount, void const* vertexData, unsigned int vertexStride, buffer_attribute_t const* layout);
	void UpdateIndices(unsigned int icount, unsigned int const* indices);

	int GetIndexCount() const { return m_indexCount; }
	int GetVertexCount() const { return m_vertexCount; }
	void UpdateIndices(std::vector<unsigned int> const& indices)
	{
		UpdateIndices((unsigned int)indices.size(), &indices[0]);
	}

	void UpdateVertices(std::vector<Vertex_PCU> const& vertices)
	{
		UpdateVertices((unsigned int)vertices.size(),
			&vertices[0],
			sizeof(Vertex_PCU),
			Vertex_PCU::LAYOUT);
	}

	void UpdateVertices(std::vector<Vertex_Lit> const& vertices)
	{
		UpdateVertices((unsigned int)vertices.size(),
			&vertices[0],
			sizeof(Vertex_Lit),
			Vertex_Lit::LAYOUT);
	}

	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices(unsigned int vcount, VERTEX_TYPE const* vertices)
	{
		UpdateVertices(vcount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT);
	}
	VertexBuffer* GetVertexBuffer() { return m_vertices; }
	IndexBuffer* GetIndexBuffer() { return m_indices; }
public:
	VertexBuffer* m_vertices = nullptr;		// like a std::vector<VertexPCU> -> but on GPU
	IndexBuffer* m_indices = nullptr; 		// like a std::vector<uint> -> but on the GPU
	RenderContext* m_owner;
	int m_indexCount = 0;
	int m_vertexCount = 0;
};