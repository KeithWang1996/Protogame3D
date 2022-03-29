#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_Lit.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "ThirdParty/Mikkt/mikktspace.h"
#include <iostream>
#include <fstream>

void AddVerticeAndIndicesToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, std::vector<Vertex_PCU>& vertsToAdd, std::vector<unsigned int>& indicesToAdd)
{
	int startIndex = (int)verts.size();
	verts.insert(verts.begin(), vertsToAdd.begin(), vertsToAdd.end());
	for (int indicesIndex = 0; indicesIndex < indicesToAdd.size(); ++indicesIndex)
	{
		indices.push_back(indicesToAdd[indicesIndex] + startIndex);
	}
}

void AddAABB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB2 bound, AABB2 uvs, float z, Rgba8 color)
{
	int startIndex = (int)verts.size();
	verts.push_back(Vertex_PCU(Vec3(bound.mins, z), color, uvs.mins));
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, z), color, Vec2(uvs.maxs.x, uvs.mins.y)));
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, z), color, Vec2(uvs.mins.x, uvs.maxs.y)));
	verts.push_back(Vertex_PCU(Vec3(bound.maxs, z), color, uvs.maxs));

	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 3);
}

void AddOBB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, OBB2 bound, Vec2 uvMins, Vec2 uvMaxs, Rgba8 color)
{
	int startIndex = (int)verts.size();
	Vec2 points[4];
	bound.GetCornerPositions(points);

	verts.push_back(Vertex_PCU(Vec3(points[0], 0.f), color, uvMins));
	verts.push_back(Vertex_PCU(Vec3(points[1], 0.f), color, Vec2(uvMaxs.x, uvMins.y)));
	verts.push_back(Vertex_PCU(Vec3(points[2], 0.f), color, Vec2(uvMins.x, uvMaxs.y)));
	verts.push_back(Vertex_PCU(Vec3(points[3], 0.f), color, uvMaxs));

	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 3);
}

void AddAABB2ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB2 bound, float z, Rgba8 color)
{
	int startIndex = (int)verts.size();
	verts.push_back(Vertex_PCU(Vec3(bound.mins, z), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, z), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, z), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex_PCU(Vec3(bound.maxs, z), color, Vec2(1.f, 1.f)));

	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 3);
}

void AddAABB2ToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, AABB2 bound, float z, Rgba8 color)
{
	int startIndex = (int)verts.size();
	verts.push_back(Vertex_Lit(Vec3(bound.mins, z), color, Vec2::ZERO, 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, z), color, Vec2(1.f, 0.f),
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, z), color, Vec2(0.f, 1.f),
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));
	verts.push_back(Vertex_Lit(Vec3(bound.maxs, z), color, Vec2(1.f, 1.f),
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));

	indices.push_back(startIndex + 0);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 3);
}

void AddAABB3ToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, AABB3 bound, Rgba8 color)
{
	int startIndex = (int)verts.size();
	//Front Face
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2::ZERO));			//0
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 0.f)));		//1
	verts.push_back(Vertex_PCU(Vec3(bound.maxs), color, Vec2(1.f, 1.f)));									//2
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2(0.f, 1.f)));		//3
	//Left Face
	verts.push_back(Vertex_PCU(Vec3(bound.mins), color, Vec2::ZERO));										//4
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 0.f)));		//5
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2(1.f, 1.f)));		//6
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 1.f)));		//7

	//Right Face
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2::ZERO));			//8
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(1.f, 0.f)));		//9
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 1.f)));		//10
	verts.push_back(Vertex_PCU(bound.maxs, color, Vec2(0.f, 1.f)));											//11

	//Up Face
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2::ZERO));			//12
	verts.push_back(Vertex_PCU(bound.maxs, color, Vec2(1.f, 0.f)));											//13
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 1.f)));		//14
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 1.f)));		//15

	//Down Face
	verts.push_back(Vertex_PCU(Vec3(bound.mins), color, Vec2(0.f, 0.f)));									//16
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(1.f, 0.f)));		//17
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 1.f)));		//18
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2(0.f, 1.f)));		//19

	//Back Face
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(0.f, 0.f)));		//20
	verts.push_back(Vertex_PCU(bound.mins, color, Vec2(1.f, 0.f)));											//21
	verts.push_back(Vertex_PCU(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 1.f)));		//22
	verts.push_back(Vertex_PCU(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 1.f)));		//23

	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
	{
		indices.push_back(startIndex + 0 + 4 * faceIndex);
		indices.push_back(startIndex + 1 + 4 * faceIndex);
		indices.push_back(startIndex + 3 + 4 * faceIndex);

		indices.push_back(startIndex + 1 + 4 * faceIndex);
		indices.push_back(startIndex + 2 + 4 * faceIndex);
		indices.push_back(startIndex + 3 + 4 * faceIndex);
	}
}

void AddAABB3ToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, AABB3 bound, Rgba8 color)
{
	int startIndex = (int)verts.size();
	//Up Face
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2(0.f, 0.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));									//0
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 0.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));									//1
	verts.push_back(Vertex_Lit(Vec3(bound.maxs), color, Vec2(1.f, 1.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)));									//2
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2(0.f, 1.f), 
		Vec4(1.f, 0.f, 0.f, 1.f),  Vec3(0.f, 0.f, 1.f)));									//3

	//Front Face
	verts.push_back(Vertex_Lit(Vec3(bound.mins), color, Vec2(1.f, 0.f), 
		Vec4(0.f, 0.f, 1.f, 1.f), Vec3(-1.f, 0.f, 0.f)));									//4
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 1.f),
		Vec4(0.f, 0.f, 1.f, 1.f), Vec3(-1.f, 0.f, 0.f)));									//5
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2(0.f, 1.f), 
		Vec4(0.f, 0.f, 1.f, 1.f), Vec3(-1.f, 0.f, 0.f)));									//6
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 0.f), 
		Vec4(0.f, 0.f, 1.f, 1.f), Vec3(-1.f, 0.f, 0.f)));									//7

	//Back Face
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2(0.f, 1.f), 
		Vec4(0.f, 0.f, -1.f, 1.f), Vec3(1.f, 0.f, 0.f)));									//8
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(0.f, 0.f), 
		Vec4(0.f, 0.f, -1.f, 1.f), Vec3(1.f, 0.f, 0.f)));									//9
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 0.f), 
		Vec4(0.f, 0.f, -1.f, 1.f), Vec3(1.f, 0.f, 0.f)));									//10
	verts.push_back(Vertex_Lit(bound.maxs, color, Vec2(1.f, 1.f),
		Vec4(0.f, 0.f, -1.f, 1.f), Vec3(1.f, 0.f, 0.f)));									//11

	//Left Face
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.maxs.z), color, Vec2(1.f, 1.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));									//12
	verts.push_back(Vertex_Lit(bound.maxs, color, Vec2(0.f, 1.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));									//13
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 0.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));									//14
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 0.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));									//15

	//Right Face
	verts.push_back(Vertex_Lit(Vec3(bound.mins), color, Vec2(0.f, 0.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, -1.f, 0.f)));									//16
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(1.f, 0.f),
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, -1.f, 0.f)));									//17
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.maxs.z), color, Vec2(1.f, 1.f), 
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, -1.f, 0.f)));									//18
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.mins.y, bound.maxs.z), color, Vec2(0.f, 1.f),
		Vec4(1.f, 0.f, 0.f, 1.f), Vec3(0.f, -1.f, 0.f)));									//19

	//Down Face
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.mins.y, bound.mins.z), color, Vec2(1.f, 1.f), 
		Vec4(-1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, -1.f)));									//20
	verts.push_back(Vertex_Lit(bound.mins, color, Vec2(0.f, 1.f),
		Vec4(-1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, -1.f)));									//21
	verts.push_back(Vertex_Lit(Vec3(bound.mins.x, bound.maxs.y, bound.mins.z), color, Vec2(0.f, 0.f),
		Vec4(-1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, -1.f)));									//22
	verts.push_back(Vertex_Lit(Vec3(bound.maxs.x, bound.maxs.y, bound.mins.z), color, Vec2(1.f, 0.f),
		Vec4(-1.f, 0.f, 0.f, 1.f), Vec3(0.f, 0.f, -1.f)));									//23

	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
	{
		indices.push_back(startIndex + 0 + 4 * faceIndex);
		indices.push_back(startIndex + 1 + 4 * faceIndex);
		indices.push_back(startIndex + 3 + 4 * faceIndex);

		indices.push_back(startIndex + 1 + 4 * faceIndex);
		indices.push_back(startIndex + 2 + 4 * faceIndex);
		indices.push_back(startIndex + 3 + 4 * faceIndex);
	}
}

void AddUVSphereToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int hcut, int vcut, Rgba8 color)
{
	int startIndex = (int)verts.size();
	float horizontalDegreeSection = 360.f / (float)hcut;
	float verticalDegreeSection = 180.f / (float)vcut;
	constexpr float inverseHorizontalAll = 1.f / 360.f;
	constexpr float inverseVerticalAll = 1.f / 180.f;

	float currentVerticalDegrees = -90.f;
	for (int i = 0; i < vcut + 1; ++i)
	{
		float currentHorizontalDegrees = 0.f;
		for (int j = 0; j < hcut + 1; ++j)
		{
			verts.push_back(Vertex_PCU(SphericalToCartesian(currentHorizontalDegrees, currentVerticalDegrees, radius) + center, 
				color, Vec2(1.f - currentHorizontalDegrees * inverseHorizontalAll, (currentVerticalDegrees + 90.f) * inverseVerticalAll)));
			currentHorizontalDegrees += horizontalDegreeSection;
		}
		currentVerticalDegrees += verticalDegreeSection;
	}

	for (int i = 0; i < vcut; ++i)
	{
		for (int j = 0; j < hcut; ++j)
		{
			indices.push_back(startIndex + (j + (hcut + 1) * i));
			indices.push_back(startIndex + (j + (hcut + 1) * i) + 1);
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)));

			indices.push_back(startIndex + (j + (hcut + 1) * i) + 1);
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)) + 1);
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)));
		}
	}
}

void AddUVSphereToIndexedVertexArray(std::vector<Vertex_Lit>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int hcut, int vcut, Rgba8 color)
{
	int startIndex = (int)verts.size();
	float horizontalDegreeSection = 360.f / (float)hcut;
	float verticalDegreeSection = 180.f / (float)vcut;
	constexpr float inverseHorizontalAll = 1.f / 360.f;
	constexpr float inverseVerticalAll = 1.f / 180.f;

	float currentVerticalDegrees = -90.f;
	for (int i = 0; i < vcut + 1; ++i)
	{
		float currentHorizontalDegrees = 0.f;
		for (int j = 0; j < hcut + 1; ++j)
		{
			Vec3 localPosition = SphericalToCartesian(currentHorizontalDegrees, currentVerticalDegrees, radius);
			Vertex_Lit vertex = Vertex_Lit(localPosition + center,
				color, Vec2(1.f - currentHorizontalDegrees * inverseHorizontalAll, (currentVerticalDegrees + 90.f) * inverseVerticalAll));
			vertex.SetNormal(localPosition.GetNormalized());
			vertex.SetTangent(Vec4(Vec3(SinDegrees(currentHorizontalDegrees), 0.f, -CosDegrees(currentHorizontalDegrees)).GetNormalized(), 1.f));
			verts.push_back(vertex);
			currentHorizontalDegrees += horizontalDegreeSection;
		}
		currentVerticalDegrees += verticalDegreeSection;
	}

	for (int i = 0; i < vcut; ++i)
	{
		for (int j = 0; j < hcut; ++j)
		{
			indices.push_back(startIndex + (j + (hcut + 1) * i));
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)));
			indices.push_back(startIndex + (j + (hcut + 1) * i) + 1);

			indices.push_back(startIndex + (j + (hcut + 1) * i) + 1);
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)));
			indices.push_back(startIndex + (j + (hcut + 1) * (i + 1)) + 1);
		}
	}
}

void TesselationIndexedMesh(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices)
{
	int numFaces = (int)indices.size() / 3;
	for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex)
	{
		int index1 = indices[faceIndex * 3];
		int index2 = indices[faceIndex * 3 + 1];
		int index3 = indices[faceIndex * 3 + 2];
		Vertex_PCU vert1 = verts[index1];
		Vertex_PCU vert2 = verts[index2];
		Vertex_PCU vert3 = verts[index3];

		//Between 1 and 2
		Vertex_PCU vert4 = Vertex_PCU((vert1.m_position + vert2.m_position) * 0.5f, vert1.m_color, (vert1.m_uvTexCoords + vert2.m_uvTexCoords) * 0.5f);
		//Between 2 and 3
		Vertex_PCU vert5 = Vertex_PCU((vert2.m_position + vert3.m_position) * 0.5f, vert1.m_color, (vert2.m_uvTexCoords + vert3.m_uvTexCoords) * 0.5f);
		//Between 3 and 1
		Vertex_PCU vert6 = Vertex_PCU((vert3.m_position + vert1.m_position) * 0.5f, vert1.m_color, (vert3.m_uvTexCoords + vert1.m_uvTexCoords) * 0.5f);

		int currentLastIndex = (int)verts.size();
		verts.push_back(vert4);
		verts.push_back(vert5);
		verts.push_back(vert6);
		int index4 = currentLastIndex;
		int index5 = currentLastIndex + 1;
		int index6 = currentLastIndex + 2;

		indices.push_back(index1);
		indices.push_back(index4);
		indices.push_back(index6);

		indices.push_back(index4);
		indices.push_back(index5);
		indices.push_back(index6);

		indices.push_back(index4);
		indices.push_back(index2);
		indices.push_back(index5);

		indices.push_back(index6);
		indices.push_back(index5);
		indices.push_back(index3);
	}
	for (int oldIndex = 0; oldIndex < numFaces * 3; ++oldIndex)
	{
		indices.erase(indices.begin());
	}
}

void AddCubeSphereToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float radius, int numTesselation, Rgba8 color)
{
	int startIndex = (int)verts.size();
	AddAABB3ToIndexedVertexArray(verts, indices, AABB3(center + Vec3(-1.f, -1.f, -1.f), center + Vec3(1.f, 1.f, 1.f)), color);
	for (int tesIndex = 0; tesIndex < numTesselation; ++tesIndex)
	{
		TesselationIndexedMesh(verts, indices);
	}
	for (int vertIndex = startIndex; vertIndex < verts.size(); ++vertIndex)
	{
		Vec3 normal = (verts[vertIndex].m_position - center).GetNormalized();
		verts[vertIndex].m_position = center + normal * radius;
	}
}

void AddPlaneWithSubdivisionCount(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float sizeX, float sizeZ, int xCut, int zCut, Rgba8 color)
{
	int startIndex = (int)verts.size();
	float inverseX = 1.0f / (float)xCut;
	float inverseZ = 1.0f / (float)zCut;
	float deltaX = sizeX * inverseX;
	float deltaZ = sizeZ * inverseZ;
	Vec3 currPoint = center + Vec3(-sizeX * 0.5f, 0.f, -sizeZ * 0.5f);
	for (int zIndex = 0; zIndex < zCut + 1; ++zIndex)
	{
		currPoint.x = center.x + -sizeX * 0.5f;
		for (int xIndex = 0; xIndex < xCut + 1; ++xIndex)
		{
			verts.push_back(Vertex_PCU(currPoint, color, Vec2((float)xIndex * inverseX, 1.f - (float)zIndex * inverseZ)));
			currPoint = currPoint + Vec3(deltaX, 0.f, 0.f);
		}
		currPoint = currPoint + Vec3(0.f, 0.f, deltaZ);
	}

	for (int zIndex = 0; zIndex < zCut; ++zIndex)
	{
		for (int xIndex = 0; xIndex < xCut; ++xIndex)
		{
			indices.push_back(startIndex + (xIndex + (xCut + 1) * zIndex));
			indices.push_back(startIndex + (xIndex + (xCut + 1) * zIndex) + 1);
			indices.push_back(startIndex + (xIndex + (xCut + 1) * (zIndex + 1)));

			indices.push_back(startIndex + (xIndex + (xCut + 1) * zIndex) + 1);
			indices.push_back(startIndex + (xIndex + (xCut + 1) * (zIndex + 1)) + 1);
			indices.push_back(startIndex + (xIndex + (xCut + 1) * (zIndex + 1)));
		}
	}
}

void AddSurfaceWithSubdivisionCount(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 center, float sizeX, float sizeZ, int xCut, int zCut, SURFACEFUNCTION function, Rgba8 color)
{
	int startIndex = (int)verts.size();
	AddPlaneWithSubdivisionCount(verts, indices, center, sizeX, sizeZ, xCut, zCut, color);
	for (int vertIndex = startIndex; vertIndex < verts.size(); ++vertIndex)
	{
		Vec2 uv = verts[vertIndex].m_uvTexCoords;
		Vec3 position = verts[vertIndex].m_position;
		//position = function(position.x, position.z);
		position = function(uv.x, uv.y);
		position.x = position.x * sizeX - sizeX * 0.5f;
		position.z = position.z * sizeZ - sizeZ * 0.5f;
		position.y += center.y;
		verts[vertIndex].m_position = position;
	}
}

void AddCylinderToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 start, Vec3 end, float radius, Rgba8 startColor, Rgba8 endColor)
{
	int startIndex = (int)verts.size();
	Mat44 lookAt = Mat44::CreateLookAt(start, end, Vec3(0.f, 0.f, 1.f));
	AddDiscToIndexVertexArray(verts, indices, radius, lookAt, false, startColor);
	lookAt.SetTranslation3D(end);
	AddDiscToIndexVertexArray(verts, indices, radius, lookAt, true, endColor);

	indices.push_back(startIndex + 48);
	indices.push_back(startIndex + 97);
	indices.push_back(startIndex + 1);

	indices.push_back(startIndex + 97);
	indices.push_back(startIndex + 50);
	indices.push_back(startIndex + 1);

	for (int sectionIndex = 1; sectionIndex < 48; ++sectionIndex)
	{
		indices.push_back(startIndex + sectionIndex);
		indices.push_back(startIndex + sectionIndex + 49);
		indices.push_back(startIndex + sectionIndex + 1);

		indices.push_back(startIndex + sectionIndex + 49);
		indices.push_back(startIndex + sectionIndex + 50);
		indices.push_back(startIndex + sectionIndex + 1);
	}
}

void AddConeToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 start, Vec3 end, float radius, Rgba8 color)
{
	int startIndex = (int)verts.size();
	Mat44 lookAt = Mat44::CreateLookAt(start, end, Vec3(0.f, 0.f, 1.f));
	AddDiscToIndexVertexArray(verts, indices, radius, lookAt, false, color);
	verts.push_back(Vertex_PCU(end, color, Vec2::ZERO));
	int endIndex = (int)verts.size() - 1;
	indices.push_back(endIndex - 1);
	indices.push_back(endIndex);
	indices.push_back(startIndex + 1);
	for (int sectionIndex = 1; sectionIndex < 49; ++sectionIndex)
	{
		indices.push_back(startIndex + sectionIndex);
		indices.push_back(endIndex);
		indices.push_back(startIndex + sectionIndex + 1);
	}
}

void AddBasisToIndexVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Mat44 basis, float size)
{
	Vec3 origin = basis.GetTranslation3D();
	Vec3 iBasis = basis.GetIBasis3D() * size;
	Vec3 jBasis = basis.GetJBasis3D() * size;
	Vec3 kBasis = basis.GetKBasis3D() * size;
	Vec3 iMid = origin + iBasis * 0.8f;
	Vec3 jMid = origin + jBasis * 0.8f;
	Vec3 kMid = origin + kBasis * 0.8f;

	AddCylinderToIndexedVertexArray(verts, indices, origin, iMid, 0.02f * size, Rgba8(255, 0, 0, 255), Rgba8(255, 0, 0, 255));
	AddConeToIndexedVertexArray(verts, indices, iMid, iBasis + origin, 0.05f * size, Rgba8(255, 0, 0, 255));

	AddCylinderToIndexedVertexArray(verts, indices, origin, jMid, 0.02f * size, Rgba8(0, 255, 0, 255), Rgba8(0, 255, 0, 255));
	AddConeToIndexedVertexArray(verts, indices, jMid, jBasis + origin, 0.05f * size, Rgba8(0, 255, 0, 255));

	AddCylinderToIndexedVertexArray(verts, indices, origin, kMid, 0.02f * size, Rgba8(0, 0, 255, 255), Rgba8(0, 0, 255, 255));
	AddConeToIndexedVertexArray(verts, indices, kMid, kBasis + origin, 0.05f * size, Rgba8(0, 0, 255, 255));
}

void AddQuadToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 color)
{
	int startIndex = (int)verts.size();
	verts.push_back(Vertex_PCU(p0, color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(p1, color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(p2, color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(p3, color, Vec2::ZERO));

	indices.push_back(startIndex);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);

	indices.push_back(startIndex);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);
}

void AddQuadToIndexedVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 color, Vec2 uvMins, Vec2 uvMaxs)
{
	int startIndex = (int)verts.size();
	verts.push_back(Vertex_PCU(p0, color, uvMins));
	verts.push_back(Vertex_PCU(p1, color, Vec2(uvMaxs.x, uvMins.y)));
	verts.push_back(Vertex_PCU(p2, color, uvMaxs));
	verts.push_back(Vertex_PCU(p3, color, Vec2(uvMins.x, uvMaxs.y)));

	indices.push_back(startIndex);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);

	indices.push_back(startIndex);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);
}

void AddDiscToIndexVertexArray(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indices, float radius, Mat44 transformMat, bool frontToScreen, Rgba8 color)
{
	int startIndex = (int)verts.size();
	constexpr float deltaTheta = 360.f / 48.f;
	const Vec2 uv = Vec2::ZERO;
	Vec3 centerPosition = transformMat.TransformPosition3D(Vec3(0.f, 0.f, 0.f));
	verts.push_back(Vertex_PCU(centerPosition, color, uv));
	for (int indexTriangle = 0; indexTriangle < 48; ++indexTriangle)
	{
		Vec3 pos = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)indexTriangle, radius), 0.f);
		pos = transformMat.TransformPosition3D(pos);
		Vertex_PCU point = Vertex_PCU(pos, color, uv);
		verts.push_back(point);
	}
	if (frontToScreen)
	{
		indices.push_back(startIndex);
		indices.push_back(startIndex + 1);
		indices.push_back(startIndex + 48);
		for (int indexTriangle = 1; indexTriangle < 48; ++indexTriangle)
		{
			indices.push_back(startIndex);
			indices.push_back(startIndex + indexTriangle + 1);
			indices.push_back(startIndex + indexTriangle);
		}
	}
	else
	{
		indices.push_back(startIndex);
		indices.push_back(startIndex + 48);
		indices.push_back(startIndex + 1);
		for (int indexTriangle = 1; indexTriangle < 48; ++indexTriangle)
		{
			indices.push_back(startIndex);
			indices.push_back(startIndex + indexTriangle);
			indices.push_back(startIndex + indexTriangle + 1);
		}
	}
}

void TransformVerticesWithMatrix(std::vector<Vertex_PCU>& verts, Mat44 transformMat)
{
	for (int verticesIndex = 0; verticesIndex < verts.size(); ++verticesIndex)
	{
		Vec3 position = verts[verticesIndex].m_position;
		verts[verticesIndex].m_position = transformMat.TransformPosition3D(position);
	}
}

void LoadOBJToVertexArray(std::vector<Vertex_Lit>& verts, char const* filename, mesh_import_options_t const& options)
{
	int startIndex = (int)verts.size();
	std::ifstream file(filename);
	if (!file)
	{
		return;//do a guarntee or die
	}
	std::vector<Vec3> positions;
	positions.push_back(Vec3());
	std::vector<Vec3> normals;
	normals.push_back(Vec3());
	std::vector<Vec2> uvs;
	uvs.push_back(Vec2());
	std::string str;
	//Read vt, vn, v
	while (std::getline(file, str))
	{
		if (str[0] == 'v' && str[1] == ' ')
		{
			Strings line = SplitStringOnDelimiter(str, ' ');
			if (line.size() == 5)
			{
				Vec3 pos = Vec3((float)atof(line[2].c_str()),
					(float)atof(line[3].c_str()),
					(float)atof(line[4].c_str()));
				positions.push_back(pos);
			}
			if (line.size() == 4)
			{
				Vec3 pos = Vec3((float)atof(line[1].c_str()),
					(float)atof(line[2].c_str()),
					(float)atof(line[3].c_str()));
				positions.push_back(pos);
			}
		}

		if (str[0] == 'v' && str[1] == 'n')
		{
			Strings line = SplitStringOnDelimiter(str, ' ');
			if (line.size() == 4)
			{
				Vec3 nor = Vec3((float)atof(line[1].c_str()),
					(float)atof(line[2].c_str()),
					(float)atof(line[3].c_str()));
				normals.push_back(nor);
			}
		}
		if (str[0] == 'v' && str[1] == 't')
		{
			Strings line = SplitStringOnDelimiter(str, ' ');
			if (line.size() >= 3)
			{
				float u = (float)atof(line[1].c_str());
				float v = (float)atof(line[2].c_str());
				if (options.invert_v)
				{
					v = 1.f - v;
				}
				Vec2 uv = Vec2(u, v);
				uvs.push_back(uv);
			}
		}

		if (str[0] == 'f' && str[1] == ' ')
		{
			Strings line = SplitStringOnDelimiter(str, ' ');
			if (line[line.size() - 1] == "")
			{
				line.pop_back();
			}
			if (line.size() == 5)//square
			{
				//
				Strings vertexInfo1 = SplitStringOnDelimiter(line[1], '/');
				Strings vertexInfo2 = SplitStringOnDelimiter(line[2], '/');
				Strings vertexInfo3 = SplitStringOnDelimiter(line[3], '/');
				Strings vertexInfo4 = SplitStringOnDelimiter(line[4], '/');
				if (normals.size() == 1)
				{
					//sequence
					//v/vt/
					Vertex_Lit vertex1 = Vertex_Lit(positions[atoi(vertexInfo1[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo1[1].c_str())]);

					Vertex_Lit vertex2 = Vertex_Lit(positions[atoi(vertexInfo2[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo2[1].c_str())]);

					Vertex_Lit vertex3 = Vertex_Lit(positions[atoi(vertexInfo3[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo3[1].c_str())]);

					Vertex_Lit vertex4 = Vertex_Lit(positions[atoi(vertexInfo4[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo4[1].c_str())]);

					if (options.generate_normals)
					{
						Vec3 edge12 = vertex2.m_position - vertex1.m_position;
						Vec3 edge13 = vertex3.m_position - vertex1.m_position;
						Vec3 normal = CrossProduct3D(edge12, edge13).GetNormalized();

						vertex1.SetNormal(normal);
						vertex2.SetNormal(normal);
						vertex3.SetNormal(normal);
						vertex4.SetNormal(normal);
					}

					verts.push_back(vertex1);
					verts.push_back(vertex2);
					verts.push_back(vertex3);

					verts.push_back(vertex3);
					verts.push_back(vertex4);
					verts.push_back(vertex1);
				}
				else
				{
					Vertex_Lit vertex1 = Vertex_Lit(positions[atoi(vertexInfo1[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo1[1].c_str())],
						Vec4(0.f, 0.f, 0.f, 1.f),
						normals[atoi(vertexInfo1[2].c_str())]);

					Vertex_Lit vertex2 = Vertex_Lit(positions[atoi(vertexInfo2[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo2[1].c_str())],
						Vec4(0.f, 0.f, 0.f, 1.f),
						normals[atoi(vertexInfo2[2].c_str())]);

					Vertex_Lit vertex3 = Vertex_Lit(positions[atoi(vertexInfo3[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo3[1].c_str())],
						Vec4(0.f, 0.f, 0.f, 1.f),
						normals[atoi(vertexInfo3[2].c_str())]);

					Vertex_Lit vertex4 = Vertex_Lit(positions[atoi(vertexInfo4[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo4[1].c_str())],
						Vec4(0.f, 0.f, 0.f, 1.f),
						normals[atoi(vertexInfo4[2].c_str())]);
// 
					verts.push_back(vertex1);
					verts.push_back(vertex2);
					verts.push_back(vertex3);

					verts.push_back(vertex3);
					verts.push_back(vertex4);
					verts.push_back(vertex1);
					//sequence
					//v/vt/vn
				}
			}
			else if (line.size() == 4)//triangle
			{
				Strings vertexInfo1 = SplitStringOnDelimiter(line[1], '/');
				Strings vertexInfo2 = SplitStringOnDelimiter(line[2], '/');
				Strings vertexInfo3 = SplitStringOnDelimiter(line[3], '/');
				if (normals.size() == 1)
				{
					//sequence
					//v/vt/
					Vertex_Lit vertex1 = Vertex_Lit(positions[atoi(vertexInfo1[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo1[1].c_str())]);

					Vertex_Lit vertex2 = Vertex_Lit(positions[atoi(vertexInfo2[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo2[1].c_str())]);

					Vertex_Lit vertex3 = Vertex_Lit(positions[atoi(vertexInfo3[0].c_str())],
						Rgba8::WHITE,
						uvs[atoi(vertexInfo3[1].c_str())]);

					if (options.generate_normals)
					{
						Vec3 edge12 = vertex2.m_position - vertex1.m_position;
						Vec3 edge13 = vertex3.m_position - vertex1.m_position;
						Vec3 normal = CrossProduct3D(edge12, edge13).GetNormalized();

						vertex1.SetNormal(normal);
						vertex2.SetNormal(normal);
						vertex3.SetNormal(normal);
					}

					verts.push_back(vertex1);
					verts.push_back(vertex2);
					verts.push_back(vertex3);
				}
				else
				{
					for (int i = 1; i < 4; ++i)
					{
						Strings vertexInfo = SplitStringOnDelimiter(line[i], '/');
						Vertex_Lit vertex = Vertex_Lit(positions[atoi(vertexInfo[0].c_str())],
							Rgba8::WHITE,
							uvs[atoi(vertexInfo[1].c_str())],
							Vec4(0.f, 0.f, 0.f, 1.f),
							normals[atoi(vertexInfo[2].c_str())]);
						verts.push_back(vertex);
					}
				}
			}
		}
	}
	if (options.generate_tangents)
	{
		//Generate tangents here
		GenerateTangentsForVertexArray(verts);
	}
	if (options.invert_winding_order)
	{
		std::reverse(verts.begin() + startIndex, verts.end());
	}
	for (int i = startIndex; i < verts.size(); ++i)
	{
		Vec3 position = verts[i].m_position;
		Vec3 tangent = verts[i].m_tangent.XYZ();
		Vec3 normal = verts[i].m_normal;

		position = options.transform.TransformPosition3D(position);
		tangent = options.transform.TransformVector3D(tangent);
		normal = options.transform.TransformVector3D(normal);

		verts[i].m_position = position;

		verts[i].m_tangent.x = tangent.x;
		verts[i].m_tangent.y = tangent.y;
		verts[i].m_tangent.z = tangent.z;

		verts[i].m_normal = normal;
	}

}

//Tangent generation code
static int GetNumFaces(SMikkTSpaceContext const* context)
{
	// if you had index buffer
	std::vector<Vertex_Lit>& vertices = *(std::vector<Vertex_Lit>*)(context->m_pUserData);
	return (int)vertices.size() / 3;
}

//-----------------------------------------------------------------
static int GetNumberOfVerticesForFace(SMikkTSpaceContext const* pContext, const int iFace)
{
	UNUSED(pContext);
	UNUSED(iFace);
	return 3;
}

//-----------------------------------------------------------------
static void GetPositionForFaceVert(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
	std::vector<Vertex_Lit>& vertices = *(std::vector<Vertex_Lit>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get out position
	Vec3 outPos = vertices[indexInVertexArray].m_position;

	fvPosOut[0] = outPos.x;
	fvPosOut[1] = outPos.y;
	fvPosOut[2] = outPos.z;
}

//-----------------------------------------------------------------
static void GetNormalForFaceVert(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
	std::vector<Vertex_Lit>& vertices = *(std::vector<Vertex_Lit>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get out position
	Vec3 outPos = vertices[indexInVertexArray].m_normal;

	fvNormOut[0] = outPos.x;
	fvNormOut[1] = outPos.y;
	fvNormOut[2] = outPos.z;
}

//-----------------------------------------------------------------
static void GetUVForFaceVert(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
	std::vector<Vertex_Lit>& vertices = *(std::vector<Vertex_Lit>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get out position
	Vec2 outPos = vertices[indexInVertexArray].m_uvTexCoords;

	fvTexcOut[0] = outPos.x;
	fvTexcOut[1] = outPos.y;
}

//-----------------------------------------------------------------
static void SetTangent(const SMikkTSpaceContext* pContext,
	const float fvTangent[],
	const float fSign,
	const int iFace, const int iVert)
{
	std::vector<Vertex_Lit>& vertices = *(std::vector<Vertex_Lit>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;
	vertices[indexInVertexArray].m_tangent = Vec4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
}

void GenerateTangentsForVertexArray(std::vector<Vertex_Lit>& vertices)
{
	SMikkTSpaceInterface interface;

	// How does MikkT get info
	interface.m_getNumFaces = GetNumFaces;
	interface.m_getNumVerticesOfFace = GetNumberOfVerticesForFace;

	interface.m_getPosition = GetPositionForFaceVert;
	interface.m_getNormal = GetNormalForFaceVert;
	interface.m_getTexCoord = GetUVForFaceVert;

	// MikkT telling US info
	interface.m_setTSpaceBasic = SetTangent;
	interface.m_setTSpace = nullptr;


	// Next, the context!  
	// Encapsulate ONE instance of running the algorithm
	SMikkTSpaceContext context;
	context.m_pInterface = &interface;
	context.m_pUserData = &vertices;

	// RUN THE ALGO
	genTangSpaceDefault(&context);
}
