#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_VEC2,
	BUFFER_FORMAT_VEC3,
	BUFFER_FORMAT_VEC4,
	BUFFER_FORMAT_R8G8B8A8_UNORM,
};

struct buffer_attribute_t
{
	char const* name = ""; 								// used to link to a D3D11 shader
	// uint location; 									// used to link to a GL/Vulkan shader 
	eBufferFormatType type = BUFFER_FORMAT_VEC2;		// what data are we describing
	unsigned offset = 0; 								// where is it relative to the start of a vertex

	buffer_attribute_t(char const* n, eBufferFormatType t, unsigned int o);
	buffer_attribute_t() = default;
};

struct Vertex_PCU
{
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;
	static const buffer_attribute_t LAYOUT[];

public:
	Vertex_PCU();
	~Vertex_PCU();
	explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords );
};