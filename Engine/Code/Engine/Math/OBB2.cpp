#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
OBB2::OBB2(const OBB2& copyFrom)
{
	m_center = copyFrom.m_center;
	m_halfDimensions = copyFrom.m_halfDimensions;
	m_iBasis = copyFrom.m_halfDimensions;
}

OBB2::OBB2(const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal)
	: m_center(center)
	, m_halfDimensions(fullDimensions * 0.5f)
	, m_iBasis(iBasisNormal)
{}

OBB2::OBB2(const Vec2& center, const Vec2& fullDimensions, float orientationDegrees)
	: m_center(center)
	, m_halfDimensions(fullDimensions * 0.5f)
{
	Vec2 iBasisNormal = Vec2::MakeFromPolarDegrees(orientationDegrees, 1.f);
	m_iBasis = iBasisNormal;
}

OBB2::OBB2(const AABB2& asAxisAlignedBox, float orientationDegrees)
{
	m_center = asAxisAlignedBox.GetCenter();
	m_halfDimensions = asAxisAlignedBox.GetDimensions() * 0.5f;
	Vec2 iBasisNormal = Vec2::MakeFromPolarDegrees(orientationDegrees, 1.f);
	m_iBasis = iBasisNormal;
}

bool OBB2::IsPointInside(const Vec2& point) const
{
	Vec2 localPoint = GetLocalPositionOf(point);
	AABB2 localAABB2 = GetLocalAABB2();
	return localAABB2.IsPointInside(localPoint);
}

const Vec2 OBB2::GetCenter() const
{
	return m_center;
}

const Vec2 OBB2::GetDimensions() const
{
	return m_halfDimensions * 2.f;
}

const Vec2 OBB2::GetIBasisNormal() const
{
	return m_iBasis.GetNormalized();
}

const Vec2 OBB2::GetJBasisNormal() const
{
	return m_iBasis.GetRotated90Degrees().GetNormalized();
}

float OBB2::GetOrientationDegrees() const
{
	return m_iBasis.GetAngleDegrees();
}

const Vec2 OBB2::GetNearestPoint(const Vec2& referencePos) const
{
	Vec2 localPosition = GetLocalPositionOf(referencePos);
	AABB2 localAABB2 = GetLocalAABB2();
	Vec2 nearestPoint = localAABB2.GetNearestPoint(localPosition); //local nearest
	return GetWorldPositionOf(nearestPoint);
}
const Vec2 OBB2::GetPointAtUV(const Vec2& uvCoordsZeroToOne) const
{
	AABB2 localAABB2 = GetLocalAABB2();
	Vec2 localPoint = localAABB2.GetPointAtUV(uvCoordsZeroToOne);
	return GetWorldPositionOf(localPoint);
}
const Vec2 OBB2::GetUVForPoint(const Vec2& point) const
{
	AABB2 localAABB2 = GetLocalAABB2();
	Vec2 localPoint = GetLocalPositionOf(point);
	return localAABB2.GetUVForPoint(localPoint);
}

float OBB2::GetOuterRadius() const
{
	return GetLocalAABB2().GetOuterRadius();
}
float OBB2::GetInnerRadius() const
{
	return GetLocalAABB2().GetInnerRadius();
}
void OBB2::GetCornerPositions(Vec2* out_fourPoints) const
{
	GetLocalAABB2().GetCornerPositions(out_fourPoints);
	for (int pointIndex = 0; pointIndex < 4; ++pointIndex)
	{
		out_fourPoints[pointIndex] = GetWorldPositionOf(out_fourPoints[pointIndex]);
	}
}

const AABB2 OBB2::GetLocalAABB2() const
{
	return AABB2(-m_halfDimensions.x, -m_halfDimensions.y, m_halfDimensions.x, m_halfDimensions.y);
}
void OBB2::Translate(const Vec2& translation)
{
	m_center += translation;
}

void OBB2::SetCenter(const Vec2& newCenter)
{
	m_center = newCenter;
}

void OBB2::SetDimensions(const Vec2& newDimensions)
{
	m_halfDimensions = newDimensions * .5f;
}

void OBB2::SetOrientationDegrees(float completelyNewAbsoluteOrientation)
{
	m_iBasis = Vec2::MakeFromPolarDegrees(completelyNewAbsoluteOrientation, 1.f);
}

void OBB2::RotateByDegrees(float relativeRotationDegrees)
{
	m_iBasis.RotateDegrees(relativeRotationDegrees);
}

void OBB2::StretchToIncludePoint(const Vec2& point)
{
	AABB2 localAABB2 = GetLocalAABB2();
	Vec2 localPoint = GetLocalPositionOf(point);
	localAABB2.StretchToIncludePoint(localPoint);
	Vec2 localCenter = localAABB2.GetCenter();
	m_center = GetWorldPositionOf(localCenter);
	m_halfDimensions = localAABB2.GetDimensions() * 0.5f;
}

void OBB2::Fix()
{
	m_halfDimensions = Vec2(abs(m_halfDimensions.x), abs(m_halfDimensions.y));
	if (m_iBasis.GetLength() <= 0.001f)
	{
		m_iBasis = Vec2(1.f, 0.f);
	}
	else
	{
		m_iBasis.Normalize();
	}
}

void OBB2::operator=(const OBB2& assignFrom)
{
	m_center = assignFrom.m_center;
	m_halfDimensions = assignFrom.m_halfDimensions;
	m_iBasis = assignFrom.m_iBasis;
}

const Vec2 OBB2::GetLocalPositionOf(const Vec2& point) const
{
	float newX = point.x - m_center.x;
	float newY = point.y - m_center.y;
	Vec2 newPoint(newX, newY);
	newPoint.RotateDegrees(-GetOrientationDegrees());
	return newPoint;
}

const Vec2 OBB2::GetWorldPositionOf(const Vec2& localPoint) const
{
	Vec2 jBasis = GetJBasisNormal();
	float newX = localPoint.x * m_iBasis.x + localPoint.y * jBasis.x;
	float newY = localPoint.x * m_iBasis.y + localPoint.y * jBasis.y;
	Vec2 newPoint(newX, newY);
	newPoint += m_center;
	return newPoint;
}