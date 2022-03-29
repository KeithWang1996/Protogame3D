#pragma once
#include "Engine/Core/EngineCommon.hpp"
struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;
	static const AABB2 ZERO_TO_ONE;
public:
	~AABB2() = default;
	AABB2() = default;
	AABB2(const AABB2& copyFrom);
	explicit AABB2(const Vec2& paraMins, const Vec2& paraMaxs);
	explicit AABB2(float minX, float minY, float maxX, float maxY);

	bool IsPointInside(const Vec2& point) const;
	const Vec2 GetCenter() const;
	const Vec2 GetDimensions() const;
	const Vec2 GetNearestPoint(const Vec2& referencePosition) const;
	const Vec2 GetPointAtUV(const Vec2& uvCoordsZeroToOne) const;
	const Vec2 GetUVForPoint(const Vec2& point) const;
	AABB2 GetBoxWithin(const Vec2& dimensions, const Vec2& alignment) const;
	AABB2 GetBoxAtLeft(float fractionOfWidth, float additionalWidth = 0.f) const;
	AABB2 GetBoxAtRight(float fractionOfWidth, float additionalWidth = 0.f) const;
	AABB2 GetBoxAtBottom(float fractionOfHeight, float additionalHeight = 0.f) const;
	AABB2 GetBoxAtTop(float fractionOfHeight, float additionalHeight = 0.f) const;
	float GetOuterRadius() const;
	float GetInnerRadius() const;
	void GetCornerPositions(Vec2* out_fourPoints) const;

	void Translate(const Vec2& translation);
	void SetCenter(const Vec2& newCenter);
	void SetDimensions(const Vec2& newDimensions);
	void StretchToIncludePoint(const Vec2& point);
	void FitWithinBounds(const AABB2& bounds);
	void FitTightlyWithinBounds(const AABB2& bounds, const Vec2& alignment = ALIGN_CENTERED);//move alignment const to engine common
	void Resize(const Vec2& newDimensions, const Vec2& pivotAnchorAlignment = ALIGN_CENTERED);
	void AlignWithinBox(const AABB2& box, const Vec2& alignment);
	AABB2 CarveBoxOffLeft(float fractionOfWidth, float additionalWidth = 0.f);
	AABB2 CarveBoxOffRight(float fractionOfWidth, float additionalWidth = 0.f);
	AABB2 CarveBoxOffBottom(float fractionOfHeight, float additionalHeight = 0.f);
	AABB2 CarveBoxOffTop(float fractionOfHeight, float additionalHeight = 0.f);
	void SetFromText(const char* text);
	bool operator==(const AABB2& compareWith) const;
	void operator=(const AABB2& assignFrom);
};