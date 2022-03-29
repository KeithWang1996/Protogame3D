#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

DiscCollider2D::DiscCollider2D(Physics2D* system, Vec2 const& localPosition, float radius)
	: Collider2D(system)
	, m_localPosition(localPosition)
	, m_radius(radius)
{
	m_type = COLLIDER2D_DISC;
	m_bound.m_center = m_worldPosition;
	m_bound.m_radius = m_radius;
}

void DiscCollider2D::UpdateWorldShape()
{
	if (m_rigidbody == nullptr)
	{
		m_worldPosition = m_localPosition;
		m_worldRotation = m_localRotation;
	}
	else
	{
		m_worldPosition = m_localPosition + m_rigidbody->m_worldPosition;
		m_worldRotation = m_localRotation + m_rigidbody->m_RotationInRadians;
	}
	m_bound.m_center = m_worldPosition;
}

Vec2 DiscCollider2D::GetClosestPoint(Vec2 pos) const
{
	return GetNearestPointOnDisc2D(pos, m_worldPosition, m_radius);
}

bool DiscCollider2D::Contains(Vec2 pos) const
{
	return IsPointInsideDisk2D(pos, m_worldPosition, m_radius);
}

bool DiscCollider2D::IntersectsPlane(Plane2D const& plane) const
{
	return DoDiscPlaneOverlap(m_worldPosition, m_radius, plane);
}

void DiscCollider2D::AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor)
{
	std::vector<Vertex_PCU> discVertices;
	AddVertsForDisc(discVertices, Vec2::ZERO, Vec2::ZERO, m_radius, 0.f, fillColor);
	AddVertsForCircle(discVertices, Vec2::ZERO, m_radius, m_radius * 0.05f, borderColor);
	AddVertsForSegment(discVertices, Vec2::ZERO, Vec2(m_radius, 0.f), m_radius * 0.05f, borderColor);
	TransformVertexArray((int)discVertices.size(), discVertices.data(), 1.f, ConvertRadiansToDegrees(m_worldRotation), m_worldPosition);
	AddVerts(vertices, discVertices);
	Collider2D::AddVerticesToDebugRender(vertices, borderColor, fillColor);
}

float DiscCollider2D::CalculateMoment(float mass)
{
	return mass * m_radius * m_radius * 0.5f;
}