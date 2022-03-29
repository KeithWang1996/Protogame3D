#pragma once
#include "Engine/Math/Vec2.hpp"

struct Segment2D
{
public:
	Vec2 pointA = Vec2::ZERO;
	Vec2 pointB = Vec2::ZERO;
public:
	Segment2D(Vec2 const& A, Vec2 const& B);
};