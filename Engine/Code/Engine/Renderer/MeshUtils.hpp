#pragma once
#include "Engine/Math/Mat44.hpp"
#include <vector>
struct Vec3;
typedef Vec3(*SURFACEFUNCTION)(float, float);
struct AABB2;
struct AABB3;
struct OBB2;
struct Vertex_PCU;
struct Vertex_Lit;
struct Vec3;
struct Rgba8;
struct Mat44;
struct mesh_import_options_t
{
	Mat44 transform				= Mat44::IDENTITY;
	bool invert_v				= false;
	bool invert_winding_order	= false;
	bool generate_normals		= false;
	bool generate_tangents		= false;
	bool clean					= false;
};
void AddVerticeAndIndicesToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, std::vector<Vertex_PCU>& vertsToAdd, std::vector<unsigned int>& indicesToAdd);
void AddAABB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB2 bound, float z, Rgba8 color);
void AddAABB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB2 bound, AABB2 uvs, float z, Rgba8 color);
void AddOBB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, OBB2 bound, Vec2 uvMins, Vec2 uvMaxs, Rgba8 color);
void AddAABB3ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB3 bound, Rgba8 color);
void AddUVSphereToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int hcut, int vcut, Rgba8 color);
void TesselationIndexedMesh(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices);
void AddCubeSphereToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int numTesselation, Rgba8 color);
void AddPlaneWithSubdivisionCount(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float sizeX, float sizeZ, int xCut, int zCut, Rgba8 color);
void AddSurfaceWithSubdivisionCount(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float sizeX, float sizeZ, int xCut, int zCut, SURFACEFUNCTION function, Rgba8 color);
void AddCylinderToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 start, Vec3 end, float radius, Rgba8 startColor, Rgba8 endColor);
void AddDiscToIndexVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, float radius, Mat44 transformMat, bool frontToScreen, Rgba8 color);
void AddQuadToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 color);
void AddQuadToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 color, Vec2 uvMins, Vec2 uvMaxs);
void AddConeToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 start, Vec3 end, float radius, Rgba8 color);
void AddBasisToIndexVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Mat44 basis, float size = 1.f);
void TransformVerticesWithMatrix(std::vector<Vertex_PCU>& verts, Mat44 transformMat);

void AddAABB3ToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, AABB3 bound, Rgba8 color);
void AddAABB2ToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, AABB2 bound, float z, Rgba8 color);
void AddUVSphereToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int hcut, int vcut, Rgba8 color);

void LoadOBJToVertexArray(std::vector<Vertex_Lit>& verts, char const* filename, mesh_import_options_t const& options);
void GenerateTangentsForVertexArray(std::vector<Vertex_Lit>& vertices);