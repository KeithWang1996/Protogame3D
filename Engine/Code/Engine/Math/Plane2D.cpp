#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"

Plane2D::Plane2D(Vec2 const& n, float distance)
	: normal(n)
	, distanceFromOriginAlongNormal(distance)
{}

Plane2D::Plane2D(Vec2 const& n, Vec2 const& pointOnPlane)
	: normal(n)
	, distanceFromOriginAlongNormal(DotProduct2D(pointOnPlane, n))
{}

float Plane2D::GetDistance(Vec2 position) const
{
	float lengthOnNormal = GetProjectedLength2D(position, normal);
	return (float)abs(lengthOnNormal - distanceFromOriginAlongNormal);
}

bool Plane2D::GetPointIsInFront(Vec2 point)
{
	float pointDistFromPlane = DotProduct2D(point, normal);
	float distanceFromPlane = pointDistFromPlane - distanceFromOriginAlongNormal;
	return distanceFromPlane > 0.0f;
}

float Plane2D::GetSignedDistanceFromPlane(Vec2 point)
{
	float pointDistFromPlane = DotProduct2D(point, normal);
	float distanceFromPlane = pointDistFromPlane - distanceFromOriginAlongNormal;
	return distanceFromPlane;
}