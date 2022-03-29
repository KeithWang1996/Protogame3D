#pragma once
#include "Engine/Math/Vec2.hpp"
struct AABB2;
struct OBB2
{
public:
	Vec2 m_center = Vec2::ZERO;
	Vec2 m_halfDimensions = Vec2(.5f, .5f);
	Vec2 m_iBasis = Vec2(1.f, 0.f);
public:
	// construction / destruction
	OBB2() = default;
	~OBB2() = default;

	OBB2(const OBB2& copyFrom);
	explicit OBB2(const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal = Vec2(1.f, 0.f));
	explicit OBB2(const Vec2& center, const Vec2& fullDimensions, float orientationDegrees);
	explicit OBB2(const AABB2& asAxisAlignedBox, float orientationDegrees = 0.f);

	// Accessors 
	bool IsPointInside(const Vec2& point) const;
	const Vec2 GetCenter() const;
	const Vec2 GetDimensions() const;
	const Vec2 GetIBasisNormal() const;
	const Vec2 GetJBasisNormal() const;
	float GetOrientationDegrees() const;
	const Vec2 GetNearestPoint(const Vec2& referencePos) const;
	const Vec2 GetPointAtUV(const Vec2& uvCoordsZeroToOne) const;
	const Vec2 GetUVForPoint(const Vec2& point) const;
	float GetOuterRadius() const;
	float GetInnerRadius() const;
	void GetCornerPositions(Vec2* out_fourPoints) const;

	// Mutators
	void Translate(const Vec2& translation);
	void SetCenter(const Vec2& newCenter);
	void SetDimensions(const Vec2& newDimensions);
	void SetOrientationDegrees(float completelyNewAbsoluteOrientation);
	void RotateByDegrees(float relativeRotationDegrees);
	void StretchToIncludePoint(const Vec2& point);
	void Fix();

	// Operators
	void operator=(const OBB2& assignFrom);
private:
	const Vec2 GetLocalPositionOf(const Vec2& point) const;
	const Vec2 GetWorldPositionOf(const Vec2& localPoint) const;

	const AABB2 GetLocalAABB2() const;
};