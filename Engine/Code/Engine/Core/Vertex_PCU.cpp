#include "Engine/Core/Vertex_PCU.hpp"

Vertex_PCU::Vertex_PCU()
	:m_position( Vec3(0.f, 0.f, 0.f) )
	,m_color( Rgba8() )
	,m_uvTexCoords(Vec2(0.f, 0.f))
{}

Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
{
	m_position = position;
	m_color = tint;
	m_uvTexCoords = uvTexCoords;
}

Vertex_PCU::~Vertex_PCU()
{}

const buffer_attribute_t Vertex_PCU::LAYOUT[] = {
   buffer_attribute_t("POSITION",  BUFFER_FORMAT_VEC3,      		offsetof(Vertex_PCU, m_position)),
   buffer_attribute_t("COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof(Vertex_PCU, m_color)),
   buffer_attribute_t("TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof(Vertex_PCU, m_uvTexCoords)),
   buffer_attribute_t() // end - terminator element; 
};

buffer_attribute_t::buffer_attribute_t(char const* n, eBufferFormatType t, unsigned int o)
	: name(n)
	, type(t)
	, offset(o)
{}
