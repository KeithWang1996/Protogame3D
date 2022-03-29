#include "Engine/Core/Vertex_Lit.hpp"

Vertex_Lit::Vertex_Lit(const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords,
	const Vec4& tangent, const Vec3 normal)
	: m_position(position)
	, m_color(tint)
	, m_uvTexCoords(uvTexCoords)
	, m_tangent(tangent)
	, m_normal(normal)
{}

const buffer_attribute_t Vertex_Lit::LAYOUT[] = {
   buffer_attribute_t("POSITION",  BUFFER_FORMAT_VEC3,      		offsetof(Vertex_Lit, m_position)),
   buffer_attribute_t("COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof(Vertex_Lit, m_color)),
   buffer_attribute_t("TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof(Vertex_Lit, m_uvTexCoords)),
   buffer_attribute_t("TANGENT",   BUFFER_FORMAT_VEC4,      		offsetof(Vertex_Lit, m_tangent)),
   buffer_attribute_t("NORMAL",    BUFFER_FORMAT_VEC3,      		offsetof(Vertex_Lit, m_normal)),
   buffer_attribute_t() // end - terminator element; 
};

void Vertex_Lit::SetTangent(const Vec4& tangent)
{
	m_tangent = tangent;
}

void Vertex_Lit::SetNormal(const Vec3& normal)
{
	m_normal = normal;
}