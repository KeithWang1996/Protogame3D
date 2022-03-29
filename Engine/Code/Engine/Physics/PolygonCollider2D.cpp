#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

PolygonCollider2D::PolygonCollider2D(Physics2D* system, Vec2 const& localPosition, std::vector<Vec2> const vertices, bool isGiftWrapping)
	: Collider2D(system)
	, m_localPosition(localPosition)
{
	if(isGiftWrapping)
	{ 
		std::vector<Vec2> polygonVertices;
		DoGiftWrapping(vertices, polygonVertices);
		m_polygon = new Polygon2D(polygonVertices);
	}
	else
	{
		m_polygon = new Polygon2D(vertices);
	}
	m_bound = GetTightBoundDiscOfPolygon(m_polygon->GetPoints());
	m_worldPosition = m_polygon->GetCenter();
	m_polygon->TranslateTo(Vec2::ZERO);
	GUARANTEE_OR_DIE(m_polygon->IsConvex(), "Polygon is not convex");
	GUARANTEE_OR_DIE(m_polygon->IsValid(), "Polygon is not valid");
	m_type = COLLIDER2D_POLYGON;
	m_radius = GetApproximateRadiusOfPolygon2D(*m_polygon);
}

PolygonCollider2D::~PolygonCollider2D()
{
	delete m_polygon;
	m_polygon = nullptr;
}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 previousLocation = m_worldPosition;
	float previousRotation = m_worldRotation;
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
	m_bound.m_center += m_worldPosition - previousLocation;
	m_bound.m_center = (m_bound.m_center - m_worldPosition).
		GetRotatedDegrees(ConvertRadiansToDegrees(m_worldRotation - previousRotation)) +
		m_worldPosition;
}

Vec2 PolygonCollider2D::GetClosestPoint(Vec2 pos) const
{
	Polygon2D polygon = *m_polygon;
	polygon.Rotate(ConvertRadiansToDegrees(m_worldRotation));
	polygon.TranslateTo(m_worldPosition);
	return polygon.GetClosestPoint(pos);
}

Vec2 PolygonCollider2D::GetClosestPointEdge(Vec2 pos) const
{
	Polygon2D polygon = *m_polygon;
	polygon.Rotate(ConvertRadiansToDegrees(m_worldRotation));
	polygon.TranslateTo(m_worldPosition);
	return polygon.GetClosestPointOnEdge(pos);
}


bool PolygonCollider2D::Contains(Vec2 pos) const
{
	Polygon2D polygon = *m_polygon;
	polygon.Rotate(ConvertRadiansToDegrees(m_worldRotation));
	polygon.TranslateTo(m_worldPosition);
	return polygon.Contains(pos);
}

// bool PolygonCollider2D::Intersects(Collider2D const* other) const
// {
// 	switch (other->m_type)
// 	{
// 	case COLLIDER2D_DISC:
// 		return other->Intersects(this);
// 	default:
// 		return false;
// 		break;
// 	}
// }

bool PolygonCollider2D::IntersectsPlane(Plane2D const& plane) const
{
	Polygon2D polygon = *m_polygon;
	polygon.Rotate(ConvertRadiansToDegrees(m_worldRotation));
	polygon.TranslateTo(m_worldPosition);
	return DoPolygonPlaneOverlap(polygon, plane);
}

void PolygonCollider2D::AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor)
{
	std::vector<Vertex_PCU> polyVertices;
	AddVertsForPolygon2D(polyVertices, *m_polygon, fillColor);
	Vec2 edgeStart;
	Vec2 edgeEnd;

	for (int edgeIndex = 0; edgeIndex < m_polygon->GetEdgeCount(); ++edgeIndex)
	{
		m_polygon->GetEdge(edgeIndex, edgeStart, edgeEnd);
		AddVertsForSegment(polyVertices, edgeStart, edgeEnd, m_radius * 0.03f, borderColor);//Todo, change thickness into radius
	}
	TransformVertexArray((int)polyVertices.size(), polyVertices.data(), 1.f, ConvertRadiansToDegrees(m_worldRotation), m_worldPosition);
	AddVerts(vertices, polyVertices);
	Collider2D::AddVerticesToDebugRender(vertices, borderColor, fillColor);
}

float PolygonCollider2D::CalculateMoment(float mass) 
{
	return m_polygon->GetMomentumWithoutMass() * mass;
}

Polygon2D PolygonCollider2D::GetUpdatedWorldShape() const
{
	Polygon2D polygon = *m_polygon;
	polygon.Rotate(ConvertRadiansToDegrees(m_worldRotation));
	polygon.TranslateTo(m_worldPosition);
	return polygon;
}