#pragma once
#include "Engine/Math/Vec3.hpp"
struct AABB3
{
public:
	Vec3 mins;
	Vec3 maxs;
public:
	~AABB3() = default;
	AABB3() = default;
	AABB3(const AABB3& copyFrom);
	explicit AABB3(const Vec3& paraMins, const Vec3& paraMaxs);
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	Vec3 GetCenter() const;
};