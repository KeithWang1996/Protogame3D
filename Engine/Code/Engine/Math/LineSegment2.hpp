#pragma once
#include "Engine/Math/Vec2.hpp"
struct LineSegment2
{
public:
	Vec2 m_start = Vec2::ZERO;
	Vec2 m_end = Vec2::ZERO;
public:
	LineSegment2() = default;
	explicit LineSegment2(Vec2 start, Vec2 end);
};