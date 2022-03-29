#include "Engine/Math/AABB3.hpp"
AABB3::AABB3(const AABB3& copyFrom)
{
	mins = copyFrom.mins;
	maxs = copyFrom.maxs;
}

AABB3::AABB3(const Vec3& paraMins, const Vec3& paraMaxs)
{
	mins = paraMins;
	maxs = paraMaxs;
}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	mins = Vec3(minX, minY, minZ);
	maxs = Vec3(maxX, maxY, maxZ);
}

Vec3 AABB3::GetCenter() const
{
	return (maxs + mins) * 0.5f;
}