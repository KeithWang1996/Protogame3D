#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Disc2.hpp"

Polygon2D::Polygon2D(Vec2 const* points, unsigned int pointCount)
{
	for (unsigned int vertexIndex = 0; vertexIndex < pointCount; ++vertexIndex)
	{
		m_points.push_back(points[vertexIndex]);
	}
	m_center = GetCenterOfMassOfPolygon(m_points);
}

Polygon2D::Polygon2D(std::vector<Vec2> const& points)
	: m_points(points)
{
	m_center = GetAveragePoint2D(m_points);
}

bool Polygon2D::IsValid() const
{
	if (m_points.size() > 2)
	{
		return true;
	}
	return false;
}

bool Polygon2D::IsConvex() const
{
	return IsPolygon2DConvex(m_points);
}

bool Polygon2D::Contains(Vec2 point) const
{
	return IsPointInsidePolygon2D(point, m_points);
}

float Polygon2D::GetDistance(Vec2 point) const
{
	Vec2 nearestPoint = GetNearestPointOnPolygon2D(point, m_points);
	return GetDistance2D(point, nearestPoint);
}

Vec2 Polygon2D::GetClosestPoint(Vec2 point) const
{
	return GetNearestPointOnPolygon2D(point, m_points);
}

Vec2 Polygon2D::GetClosestPointOnEdge(Vec2 point) const
{
	return GetNearestPointOnPolygon2DEdge(point, m_points);
}

int Polygon2D::GetVertexCount() const
{
	return (int)m_points.size();
}

int Polygon2D::GetEdgeCount() const
{
	return (int)m_points.size();
}

void Polygon2D::GetEdge(int idx, Vec2& outStart, Vec2& outEnd)
{
	int numVertex = GetVertexCount();
	outStart = m_points[idx % numVertex];
	outEnd = m_points[(idx + 1) % numVertex];
}
void Polygon2D::Translate(Vec2 diff)
{
	for (int vertexIndex = 0; vertexIndex < m_points.size(); ++vertexIndex)
	{
		m_points[vertexIndex] += diff;
	}
	m_center += diff;
}
void Polygon2D::TranslateTo(Vec2 dest)
{
	Vec2 diff = dest - m_center;
	Translate(diff);
}

void Polygon2D::Rotate(float rotationDegrees)
{
	for (int vertexIndex = 0; vertexIndex < m_points.size(); ++vertexIndex)
	{
		m_points[vertexIndex].RotateDegrees(rotationDegrees);
	}
}

float Polygon2D::GetMomentumWithoutMass()
{
	Vec2 startPoint = m_points[0];
	float moment = 0.f;
	float area = 0.f;
	for (int i = 1; i < m_points.size() - 1; ++i)
	{
		for (int j = i + 1; j < m_points.size(); ++j)
		{
			float thisMoment = GetMomentOfTriangle(startPoint, m_points[i], m_points[j]) * GetAreaOfTriangle(startPoint, m_points[i], m_points[j]);
			float thisArea = GetAreaOfTriangle(startPoint, m_points[i], m_points[j]);
			Vec2 thisCenter = GetTriangleCircumscribedCircle(startPoint, m_points[i], m_points[j]).m_center;
			float d = GetDistance2D(thisCenter, m_center);
			thisMoment += thisArea * d * d;
			area += thisArea;
			moment += thisMoment;
		}
	}
	return moment / area;
}

Vec2 Polygon2D::GetSupport(const Vec2& direction) const
{
	return GetSupportOfPolygon(m_points, direction);
}