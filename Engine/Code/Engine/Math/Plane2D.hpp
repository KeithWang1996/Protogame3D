#pragma once
#include "Engine/Math/Vec2.hpp"
struct Plane2D
{
public:
	Vec2 normal;
	float distanceFromOriginAlongNormal; // distance; 
public:
	Plane2D(Vec2 const& n, float distance);
	Plane2D(Vec2 const& n, Vec2 const& pointOnPlane);
	float GetDistance(Vec2 position) const;
	bool GetPointIsInFront(Vec2 point);
	float GetSignedDistanceFromPlane(Vec2 point);
};