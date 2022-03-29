#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(0.f, 0.f, 1.f, 1.f);

AABB2::AABB2(const AABB2& copyFrom)
	:mins(copyFrom.mins)
	,maxs(copyFrom.maxs)
{}

AABB2::AABB2(const Vec2& paraMins, const Vec2& paraMaxs)
	: mins(paraMins)
	, maxs(paraMaxs)
{
	if (paraMins.x > paraMaxs.x)
	{
		mins.x = paraMaxs.x;
		maxs.x = paraMins.x;
	}
	if (paraMins.y > paraMaxs.y)
	{
		mins.y = paraMaxs.y;
		maxs.y = paraMins.y;
	}
}


AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	:mins(Vec2(minX, minY))
	,maxs(Vec2(maxX, maxY))
{
	if (minX > maxX)
	{
		mins.x = maxX;
		maxs.x = minX;
	}

	if (minY > maxY)
	{
		mins.y = maxY;
		maxs.y = minY;
	}
}

bool AABB2::IsPointInside(const Vec2& point) const
{
	if (point.x < mins.x || point.x > maxs.x || point.y < mins.y || point.y > maxs.y)
	{
		return false;
	}
	return true;
}

const Vec2 AABB2::GetCenter() const
{
	return Vec2((mins.x + maxs.x) / 2.f, (mins.y + maxs.y) / 2.f);
}

const Vec2 AABB2::GetDimensions() const
{
	return Vec2(maxs.x - mins.x, maxs.y - mins.y);
}

const Vec2 AABB2::GetNearestPoint(const Vec2& referencePosition) const
{
	if (IsPointInside(referencePosition))
	{
		return referencePosition;
	}
	float nearestX = referencePosition.x;
	float nearestY = referencePosition.y;
	if (nearestX > maxs.x)
	{
		nearestX = maxs.x;
	}
	else if (nearestX < mins.x) {
		nearestX = mins.x;
	}
	if (nearestY > maxs.y)
	{
		nearestY = maxs.y;
	}
	else if(nearestY < mins.y){
		nearestY = mins.y;
	}
	return Vec2(nearestX, nearestY);

}
const Vec2 AABB2::GetPointAtUV(const Vec2& uvCoordsZeroToOne) const
{
	float x = RangeMapFloat(0.f, 1.f, mins.x, maxs.x, uvCoordsZeroToOne.x);
	float y = RangeMapFloat(0.f, 1.f, mins.y, maxs.y, uvCoordsZeroToOne.y);
	return Vec2(x, y);
}

AABB2 AABB2::GetBoxWithin(const Vec2& dimensions, const Vec2& alignment) const
{
	float boxMinX = mins.x;
	float boxMaxX = maxs.x;
	float boxMinY = mins.y;
	float boxMaxY = maxs.y;
	Vec2 selfDimensions = GetDimensions();
	if (dimensions.x < selfDimensions.x)
	{
		boxMinX = RangeMapFloat(0.f, 1.f, mins.x, mins.x + (selfDimensions.x - dimensions.x), alignment.x);
		boxMaxX = boxMinX + dimensions.x;
	}
	
	if (dimensions.y < selfDimensions.y)
	{
		boxMinY = RangeMapFloat(0.f, 1.f, mins.y, mins.y + (selfDimensions.y - dimensions.y), alignment.y);
		boxMaxY = boxMinY + dimensions.y;
	}

	return AABB2(boxMinX, boxMinY, boxMaxX, boxMaxY);
}

AABB2 AABB2::GetBoxAtLeft(float fractionOfWidth, float additionalWidth) const
{
	Vec2 dimensions = GetDimensions();
	float width = dimensions.x * fractionOfWidth + additionalWidth;
	return GetBoxWithin(Vec2(width, dimensions.y), ALIGN_CENTERED_LEFT);
}

AABB2 AABB2::GetBoxAtRight(float fractionOfWidth, float additionalWidth) const
{
	Vec2 dimensions = GetDimensions();
	float width = dimensions.x * fractionOfWidth + additionalWidth;
	return GetBoxWithin(Vec2(width, dimensions.y), ALIGN_CENTERED_RIGHT);
}

AABB2 AABB2::GetBoxAtBottom(float fractionOfHeight, float additionalHeight) const
{
	Vec2 dimensions = GetDimensions();
	float height = dimensions.y * fractionOfHeight + additionalHeight;
	return GetBoxWithin(Vec2(dimensions.x, height), ALIGN_BOTTOM_CENTERED);
}
AABB2 AABB2::GetBoxAtTop(float fractionOfHeight, float additionalHeight) const
{
	Vec2 dimensions = GetDimensions();
	float height = dimensions.y * fractionOfHeight + additionalHeight;
	return GetBoxWithin(Vec2(dimensions.x, height), ALIGN_TOP_CENTERED);
}
float AABB2::GetOuterRadius() const
{	
	return GetDimensions().GetLength() * 0.5f;
}
float AABB2::GetInnerRadius() const
{
	Vec2 dimensions = GetDimensions();
	if (dimensions.x <= dimensions.y)
	{
		return dimensions.x * 0.5f;
	}
	else
	{
		return dimensions.y * 0.5f;
	}
}
void AABB2::GetCornerPositions(Vec2* out_fourPoints) const
{
	Vec2 dimensions = GetDimensions();
	out_fourPoints[0] = GetCenter() - dimensions * 0.5f;
	out_fourPoints[1] = GetCenter() + Vec2(dimensions.x, -dimensions.y) * 0.5;
	out_fourPoints[2] = GetCenter() + Vec2(-dimensions.x, dimensions.y) * 0.5;
	out_fourPoints[3] = GetCenter() + dimensions * 0.5f;
}

const Vec2 AABB2::GetUVForPoint(const Vec2& point) const
{
	float x = RangeMapFloat(mins.x, maxs.x, 0.f, 1.f, point.x);
	float y = RangeMapFloat(mins.y, maxs.y, 0.f, 1.f, point.y);
	return Vec2(x, y);
}

void AABB2::Translate(const Vec2& translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2::SetCenter(const Vec2& newCenter)
{
	Vec2 dimensions = GetDimensions();
	mins = newCenter - dimensions / 2.f;
	maxs = newCenter + dimensions / 2.f;
}

void AABB2::SetDimensions(const Vec2& newDimensions)
{
	Vec2 center = GetCenter();
	mins = center - newDimensions / 2.f;
	maxs = center + newDimensions / 2.f;
}

void AABB2::StretchToIncludePoint(const Vec2& point)
{
	if (IsPointInside(point))
	{
		return;
	}
	if (point.x > maxs.x)
	{
		maxs.x = point.x;
	}
	else if(point.x < mins.x)
	{
		mins.x = point.x;
	}
	if (point.y > maxs.y)
	{
		maxs.y = point.y;
	}
	else if(point.y < mins.y)
	{
		mins.y = point.y;
	}
}

bool AABB2::operator==(const AABB2& compareWith) const
{
	if (mins == compareWith.mins && maxs == compareWith.maxs)
	{
		return true;
	}
	return false;
}
void AABB2::operator=(const AABB2& assignFrom)
{
	maxs = assignFrom.maxs;
	mins = assignFrom.mins;
}

void AABB2::FitWithinBounds(const AABB2& bounds)
{
	if (mins.x < bounds.mins.x)
	{
		Translate(Vec2(bounds.mins.x - mins.x, 0.f));
	}

	if (maxs.x > bounds.maxs.x)
	{
		Translate(Vec2(bounds.maxs.x - maxs.x, 0.f));
	}
	if (mins.y < bounds.mins.y)
	{
		Translate(Vec2(0.f, bounds.mins.y - mins.y));
	}

	if (maxs.y > bounds.maxs.y)
	{
		Translate(Vec2(0.f, bounds.maxs.y - maxs.y));
	}
}

void AABB2::FitTightlyWithinBounds(const AABB2& bounds, const Vec2& alignment)
{
	Vec2 dimensions = GetDimensions();
	float aspect = dimensions.x / dimensions.y;
	Vec2 outerDimensions = bounds.GetDimensions();
	float outerAspect = outerDimensions.x / outerDimensions.y;
	float newX = dimensions.x;
	float newY = dimensions.y;
	if (aspect >= outerAspect)
	{
		newX = outerDimensions.x;
		newY = newX * aspect;
		
	}
	else
	{
		newY = outerDimensions.y;
		newX = newY / aspect;
	}
	AABB2 newAABB2 = GetBoxWithin(Vec2(newX, newY), alignment);
	mins = newAABB2.mins;
	maxs = newAABB2.maxs;
}

void AABB2::Resize(const Vec2& newDimensions, const Vec2& pivotAnchorAlignment)
{
	float alignmentX = pivotAnchorAlignment.x;
	float alignmentY = pivotAnchorAlignment.y;
	Vec2 dimensions = GetDimensions();
	float diffX = newDimensions.x - dimensions.x;
	float diffY = newDimensions.y - dimensions.y;

	mins.x -= diffX * alignmentX;
	maxs.x += diffX * (1 - alignmentX);
	mins.y -= diffY * alignmentY;
	maxs.y += diffY * (1 - alignmentY);
}

void AABB2::AlignWithinBox(const AABB2& box, const Vec2& alignment)
{
	Vec2 dimensions = GetDimensions();
	Vec2 outerDimensions = box.GetDimensions();
	Vec2 newCenter = box.GetCenter();
	float diffX = outerDimensions.x - dimensions.x;
	float diffY = outerDimensions.y - dimensions.y;
	if (diffX > 0)
	{
		newCenter.x = RangeMapFloat(0.f, 1.f, newCenter.x - diffX * 0.5f, newCenter.x + diffX * 0.5f, alignment.x);
	}

	if (diffY > 0)
	{
		newCenter.y = RangeMapFloat(0.f, 1.f, newCenter.y - diffY * 0.5f, newCenter.y + diffY * 0.5f, alignment.y);
	}
	SetCenter(newCenter);
}

AABB2 AABB2::CarveBoxOffLeft(float fractionOfWidth, float additionalWidth)
{
	Vec2 dimensions = GetDimensions();
	float width = dimensions.x * fractionOfWidth + additionalWidth;
	mins.x += width;
	return GetBoxAtLeft(fractionOfWidth, additionalWidth);
}
AABB2 AABB2::CarveBoxOffRight(float fractionOfWidth, float additionalWidth)
{
	Vec2 dimensions = GetDimensions();
	float width = dimensions.x * fractionOfWidth + additionalWidth;
	maxs.x -= width;
	return GetBoxAtRight(fractionOfWidth, additionalWidth);
}
AABB2 AABB2::CarveBoxOffBottom(float fractionOfHeight, float additionalHeight)
{
	Vec2 dimensions = GetDimensions();
	float height = dimensions.y * fractionOfHeight + additionalHeight;
	mins.y += height;
	return GetBoxAtBottom(fractionOfHeight, additionalHeight);
}
AABB2 AABB2::CarveBoxOffTop(float fractionOfHeight, float additionalHeight)
{
	Vec2 dimensions = GetDimensions();
	float height = dimensions.y * fractionOfHeight + additionalHeight;
	maxs.y -= height;
	return GetBoxAtTop(fractionOfHeight, additionalHeight);
}

void AABB2::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() != 4)
	{
		return;
	}
	mins = Vec2(static_cast<float>(atof(strings[0].c_str())), static_cast<float>(atof(strings[1].c_str())));
	maxs = Vec2(static_cast<float>(atof(strings[2].c_str())), static_cast<float>(atof(strings[3].c_str())));
}