#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/Segment2D.hpp"
#include "Engine/Renderer/DebugRender.hpp"

Collider2D::Collider2D(Physics2D* system)
	: m_system(system)
{}

Collider2D::~Collider2D()
{}

void Collider2D::Destroy()
{
	m_system->DestroyCollider(this);
}

void Collider2D::MarkAsDestroyed()
{
	m_isDestroyed = true;
}

void Collider2D::SetBounciness(float bounciness)
{
	m_physicsMaterial.restitution = Clamp(bounciness, 0.f, 1.f);
}

void Collider2D::ChangeBounciness(float offset)
{
	m_physicsMaterial.restitution = Clamp(m_physicsMaterial.restitution + offset, 0.f, 1.f);
}

void Collider2D::ChangeFriction(float offset)
{
	m_physicsMaterial.friction = Clamp(m_physicsMaterial.friction + offset, 0.f, 1.f);
}

float Collider2D::GetBounceWith(Collider2D const* other)
{
	float otherBouciness = other->m_physicsMaterial.restitution;
	return m_physicsMaterial.restitution * otherBouciness;
}

float Collider2D::GetFrictionWith(Collider2D const* other)
{
	float otheFriction = other->m_physicsMaterial.friction;
	return m_physicsMaterial.friction * otheFriction;
}

bool Collider2D::Intersects(Collider2D const* other) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	Disc2 discMe = m_bound;
	Disc2 discOther = other->GetWorldBounds();
	if (!DoDiscsOverlap(discMe.m_center, discMe.m_radius, discOther.m_center, discOther.m_radius))
	{
		return false;
	}
	if (myType <= otherType) {
		int idx = otherType * NUM_COLLIDER_TYPES + myType;
		collision_check_cb check = gCollisionChecks[idx];
		return check(this, other);
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPES + otherType;
		collision_check_cb check = gCollisionChecks[idx];
		return check(other, this);
	}
}

manifold2 Collider2D::GetManifold(Collider2D const* other) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;
	Disc2 discMe = m_bound;
	Disc2 discOther = other->GetWorldBounds();
	if (!DoDiscsOverlap(discMe.m_center, discMe.m_radius, discOther.m_center, discOther.m_radius))
	{
		return manifold2();
	}
	if (myType <= otherType) {
		int idx = otherType * NUM_COLLIDER_TYPES + myType;
		collision_manifold_cb manifold = gManifolds[idx];
		return manifold(this, other);
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPES + otherType;
		collision_manifold_cb manifold = gManifolds[idx];
		manifold2 toReturn = manifold(other, this);
		toReturn.contactNormal = -toReturn.contactNormal;
		return toReturn;
	}
}

void Collider2D::AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor)
{
	UNUSED(fillColor);
	UNUSED(borderColor);
	Rgba8 xColor = Rgba8(0, 0, 255, 255);
	if (!m_rigidbody->m_physicsEnabled)
	{
		xColor = Rgba8(255, 0, 0, 255);
	}
	Vec2 worldPosition = m_rigidbody->m_worldPosition;
	//ctx->DrawSegment(worldPosition + Vec2(-0.1f, -0.1f), worldPosition + Vec2(0.1f, 0.1f), 0.02f, xColor);
	//ctx->DrawSegment(worldPosition + Vec2(-0.1f, 0.1f), worldPosition + Vec2(0.1f, -0.1f), 0.02f, xColor);
	AddVertsForSegment(vertices, worldPosition + Vec2(-0.1f, -0.1f), worldPosition + Vec2(0.1f, 0.1f), 0.02f, xColor);
	AddVertsForSegment(vertices, worldPosition + Vec2(-0.1f, 0.1f), worldPosition + Vec2(0.1f, -0.1f), 0.02f, xColor);
}

bool Collider2D::DiscVDiscCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
	if (col0->m_type != COLLIDER2D_DISC || col1->m_type != COLLIDER2D_DISC)
	{
		return false;
	}
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;
	
	Vec2 closestPoint = disc1->GetClosestPoint(disc0->m_rigidbody->m_worldPosition);
	if (GetDistance2D(disc0->m_worldPosition, closestPoint) <= disc0->m_radius)
	{
		return true;
	}
	return false;
}

bool Collider2D::DiscVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
	if (col0->m_type != COLLIDER2D_DISC || col1->m_type != COLLIDER2D_POLYGON)
	{
		return false;
	}
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly1 = (PolygonCollider2D const*)col1;

	Vec2 closestPoint = poly1->GetClosestPoint(disc0->m_rigidbody->m_worldPosition);
	if (GetDistance2D(disc0->m_worldPosition, closestPoint) <= disc0->m_radius)
	{
		return true;
	}
	return false;
}

bool Collider2D::PolygonVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
	if (col0->m_type != COLLIDER2D_POLYGON || col1->m_type != COLLIDER2D_POLYGON)
	{
		return false;
	}
	PolygonCollider2D const* poly0 = (PolygonCollider2D const*)col0;
	PolygonCollider2D const* poly1 = (PolygonCollider2D const*)col1;

	if (IsPolygonIntersectWithOtherPolygon(poly0->GetUpdatedWorldShape(), poly1->GetUpdatedWorldShape()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

Collider2D::collision_check_cb Collider2D::gCollisionChecks[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES] =
{
	/*             disc,										polygon, */
	/*    disc */  Collider2D::DiscVDiscCollisionCheck,			nullptr,
	/* polygon */  Collider2D::DiscVPolygonCollisionCheck,		Collider2D::PolygonVPolygonCollisionCheck
};

manifold2 Collider2D::GetDiscVDiscCollisionManifold(Collider2D const* col0, Collider2D const* col1)
{
	manifold2 manifold;
	if (col0->m_type != COLLIDER2D_DISC || col1->m_type != COLLIDER2D_DISC)
	{
		return manifold;
	}
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;
	Vec2 diff = disc0->m_worldPosition - disc1->m_worldPosition;
	manifold.contactEdge.pointA = disc0->m_worldPosition - diff * 0.5f;
	manifold.contactEdge.pointB = disc0->m_worldPosition - diff * 0.5f;
	manifold.contactNormal = diff.GetNormalized();
	manifold.penetration = disc0->m_radius + disc1->m_radius - diff.GetLength();
	return manifold;
}

manifold2 Collider2D::GetDiscVPolygonCollisionManifold(Collider2D const* col0, Collider2D const* col1)
{
	manifold2 manifold;
	if (col0->m_type != COLLIDER2D_DISC || col1->m_type != COLLIDER2D_POLYGON)
	{
		return manifold;
	}
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly1 = (PolygonCollider2D const*)col1;
	Vec2 closestPoint = poly1->GetClosestPointEdge(disc0->m_worldPosition);
	Vec2 diff = disc0->m_worldPosition - closestPoint;
	manifold.contactEdge.pointA = closestPoint;
	manifold.contactEdge.pointB = closestPoint;
	manifold.contactNormal = diff.GetNormalized();
	if (poly1->Contains(disc0->m_worldPosition))
	{
		manifold.penetration = disc0->m_radius + diff.GetLength();
	}
	else
	{
		manifold.penetration = disc0->m_radius - diff.GetLength();
	}
	return manifold;
}

manifold2 Collider2D::GetPolygonVPolygonCollisionManifold(Collider2D const* col0, Collider2D const* col1)
{
	manifold2 manifold;
	if (col0->m_type != COLLIDER2D_POLYGON || col1->m_type != COLLIDER2D_POLYGON)
	{
		return manifold;
	}
	PolygonCollider2D const* poly0 = (PolygonCollider2D const*)col0;
	PolygonCollider2D const* poly1 = (PolygonCollider2D const*)col1;

	Polygon2D polyA = poly0->GetUpdatedWorldShape();
	Polygon2D polyB = poly1->GetUpdatedWorldShape();
	Vec2 contactNormal = -GetContactNormalOfTwoPolygons(polyA, polyB);
	//DebugAddWorldPoint(Vec3(polyB.GetCenter(), 0.f) + Vec3(20.f, 10.f, 0.f), 1.f, Rgba8(255, 0, 0, 255), 2.f);
	manifold.penetration = contactNormal.GetLength();
	contactNormal.Normalize();
	manifold.contactNormal = contactNormal;

	//Get support of B
	Vec2 p = polyB.GetSupport(contactNormal);
	Plane2D R = Plane2D(contactNormal, p);
	std::vector<Vec2> edgePoints;
	std::vector<Vec2> polyBPoints = polyB.GetPoints();
	std::vector<Vec2> polyAPoints = polyA.GetPoints();
	//Find all points on edge
	for (int verticesIndex = 0; verticesIndex < polyBPoints.size(); ++verticesIndex)
	{
		if (R.GetDistance(polyBPoints[verticesIndex]) < 0.01f)
		{
			edgePoints.push_back(polyBPoints[verticesIndex]);
		}
	}
	//Find min and max
	Vec2 tMax = contactNormal.GetRotated90Degrees();
	float min = INFINITY;
	float max = -INFINITY;
	Vec2 edgeMax = edgePoints[0];
	Vec2 edgeMin = edgePoints[0];
	for (int verticesIndex = 0; verticesIndex < edgePoints.size(); ++verticesIndex)
	{
		float distanceOnEdge = DotProduct2D(tMax, edgePoints[verticesIndex]);
		if (distanceOnEdge > max)
		{
			max = distanceOnEdge;
			edgeMax = edgePoints[verticesIndex];
		}

		if (distanceOnEdge < min)
		{
			min = distanceOnEdge;
			edgeMin = edgePoints[verticesIndex];
		}
	}
	if (edgeMax == edgeMin)
	{
		manifold.contactEdge.pointA = edgeMax;
		manifold.contactEdge.pointB = edgeMin;
		return manifold;
	}
	Segment2D refEdge(edgeMin, edgeMax);
	Segment2D edgeA(polyAPoints[polyAPoints.size() - 1], polyAPoints[0]);
	std::vector<Vec2> contactPoints;
	if (R.GetSignedDistanceFromPlane(edgeA.pointA) < 0.f ||
		R.GetSignedDistanceFromPlane(edgeA.pointB) < 0.f )
	{
		Segment2D clipedEdgeA = ClipSegmentToSegment(edgeA, refEdge);
		if (R.GetSignedDistanceFromPlane(clipedEdgeA.pointA) < 0.f)
		{
			contactPoints.push_back(clipedEdgeA.pointA);
		}

		if (R.GetSignedDistanceFromPlane(clipedEdgeA.pointB) < 0.f)
		{
			contactPoints.push_back(clipedEdgeA.pointB);
		}
	}

	for (int edgeIndex = 0; edgeIndex < polyAPoints.size() - 1; ++edgeIndex)
	{
		edgeA = Segment2D(polyAPoints[edgeIndex], polyAPoints[edgeIndex + 1]);
		if (R.GetSignedDistanceFromPlane(edgeA.pointA) < 0.f ||
			R.GetSignedDistanceFromPlane(edgeA.pointB) < 0.f)
		{
			Segment2D clipedEdgeA = ClipSegmentToSegment(edgeA, refEdge);
			if (R.GetSignedDistanceFromPlane(clipedEdgeA.pointA) < 0.f)
			{
				contactPoints.push_back(clipedEdgeA.pointA);
			}

			if (R.GetSignedDistanceFromPlane(clipedEdgeA.pointB) < 0.f)
			{
				contactPoints.push_back(clipedEdgeA.pointB);
			}
		}
	}
	min = INFINITY;
	max = -INFINITY;
	Vec2 contactMax = contactPoints[0];
	Vec2 contactMin = contactPoints[0];
	for (int verticesIndex = 0; verticesIndex < contactPoints.size(); ++verticesIndex)
	{
		float distanceOnEdge = DotProduct2D(tMax, contactPoints[verticesIndex]);
		if (distanceOnEdge > max)
		{
			max = distanceOnEdge;
			contactMax = contactPoints[verticesIndex];
		}

		if (distanceOnEdge < min)
		{
			min = distanceOnEdge;
			contactMin = contactPoints[verticesIndex];
		}
	}
	manifold.contactEdge.pointA = contactMin;
	manifold.contactEdge.pointB = contactMax;

	return manifold;
}

Collider2D::collision_manifold_cb Collider2D::gManifolds[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES] =
{
	/*             disc,										polygon, */
	/*    disc */  Collider2D::GetDiscVDiscCollisionManifold,			nullptr,
	/* polygon */  Collider2D::GetDiscVPolygonCollisionManifold,		Collider2D::GetPolygonVPolygonCollisionManifold
};