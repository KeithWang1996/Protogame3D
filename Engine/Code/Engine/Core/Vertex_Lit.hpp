#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vertex_Lit 
{
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;

	Vec4 m_tangent;
	Vec3 m_normal;

	static const buffer_attribute_t LAYOUT[];
public:
	Vertex_Lit() = default;
	~Vertex_Lit() = default;
	explicit Vertex_Lit(const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords, 
		const Vec4& tangent = Vec4(0.f, 0.f, 0.f, 0.f) ,const Vec3 normal = Vec3(0.f, 0.f, 0.f));
	void SetTangent(const Vec4& tangent);
	void SetNormal(const Vec3& normal);
};