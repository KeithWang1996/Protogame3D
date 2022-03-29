#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"

Capsule2::Capsule2(Vec2 start, Vec2 end, float radius)
	: m_start(start)
	, m_end(end)
	, m_radius(radius)
{}

const Vec2 Capsule2::GetCenter() const
{
	return (m_start + m_end) * 0.5f;
}

const OBB2 Capsule2::GetOBB2AlongTheLine()const
{
	Vec2 boxCenter = GetCenter();
	float boxDimensionX = m_radius * 2.f;
	float boxDimensionY = (m_end - m_start).GetLength();
	Vec2 boxIBasisNormalized = (m_end - m_start).GetNormalized().GetRotatedMinus90Degrees();
	return OBB2(boxCenter, Vec2(boxDimensionX, boxDimensionY), boxIBasisNormalized);
}