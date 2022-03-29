#pragma once
#include "Engine/Math/Vec2.hpp"
struct OBB2;

struct Capsule2
{
public:
	Vec2 m_start = Vec2::ZERO;
	Vec2 m_end = Vec2::ZERO;
	float m_radius = 0.f;
public:
	Capsule2() = default;
	explicit Capsule2(Vec2 start, Vec2 end, float radius);
	const Vec2 GetCenter() const;
	const OBB2 GetOBB2AlongTheLine()const;
};