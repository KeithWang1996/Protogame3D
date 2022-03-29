#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Segment2D.hpp"
#include <vector>

float	ConvertDegreesToRadians( float degrees )
{
	return degrees / 180.f * 3.14159f;
}

float	ConvertRadiansToDegrees( float radians )
{
	return radians / 3.14159f * 180.f;
}

float	CosDegrees( float degrees )
{
	return cosf(ConvertDegreesToRadians(degrees));
}
float   SinDegrees( float degrees )
{
	return sinf(ConvertDegreesToRadians(degrees));
}

float	ACosDegrees(float cos)
{
	return ConvertRadiansToDegrees(acosf(cos));
}

float   ASinDegrees(float sin)
{
	return ConvertRadiansToDegrees(asinf(sin));
}

float	Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

float   GetShortestAngularDisplacement(float fromDegrees, float toDegrees)
{
	while (fromDegrees > 360.f)
	{
		fromDegrees -= 360.f;
	}
	while (fromDegrees < 0.f)
	{
		fromDegrees += 360.f;
	}
	while (toDegrees > 360.f)
	{
		toDegrees -= 360.f;
	}
	while (toDegrees < 0.f)
	{
		toDegrees += 360.f;
	}
	float diff = toDegrees - fromDegrees;

	if (fabsf(diff) > 180.f)
	{
		if (diff > 0.f)
		{
			return fabsf(diff) - 360.f;
		}
		else
		{
			return 360.f - fabsf(diff);
		}
	}
	return diff;
}

float	GetTurnedToward(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	float diff = GetShortestAngularDisplacement(currentDegrees, goalDegrees);
	if (fabsf(diff) < maxDeltaDegrees)
	{
		return goalDegrees;
	}
	else if (diff < 0.f)
	{
		return currentDegrees - maxDeltaDegrees;
	}
	else
	{
		return currentDegrees + maxDeltaDegrees;
	}
}

bool	IsPointInForwardSector2D(const Vec2& point, const Vec2& start, float maxDist, const Vec2& fwdDir, float apertureDegrees)
{
	if ((point - start).GetLength() > maxDist)
	{
		return false;
	}
	float diffDegees = GetAngleDegreesBetweenVectors2D((point - start), fwdDir);
	if (diffDegees < apertureDegrees)
	{
		return true;
	}
	return false;
}

bool	IsPointInForwardSector2D(const Vec2& point, const Vec2& start, float fwdDegrees, float apertureDegrees, float maxDist)
{
	if ((point - start).GetLength() > maxDist)
	{
		return false;
	}
	float diffDegees = fabsf(GetShortestAngularDisplacement(fwdDegrees, (point - start).GetAngleDegrees()));
	if (diffDegees < apertureDegrees * 0.5f)
	{
		return true;
	}
	return false;
}

float   GetDistance2D( const Vec2& positionA, const Vec2& positionB )
{
	return sqrtf(GetDistanceSquared2D(positionA, positionB));
}
float	GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB )
{
	float xDiff = positionB.x - positionA.x;
	float yDiff = positionB.y - positionA.y;
	return xDiff * xDiff + yDiff * yDiff;
}
float   GetDistance3D( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf(GetDistanceSquared3D(positionA, positionB));
}
float	GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	float xDiff = positionB.x - positionA.x;
	float yDiff = positionB.y - positionA.y;
	float zDiff = positionB.z - positionA.z;
	return xDiff * xDiff + yDiff * yDiff + zDiff * zDiff;
}
float   GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf(GetDistanceXYSquared3D(positionA, positionB));
}
float	GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	float xDiff = positionB.x - positionA.x;
	float yDiff = positionB.y - positionA.y;
	return xDiff * xDiff + yDiff * yDiff;
}

bool	DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distance = GetDistance2D(centerA, centerB);
	float sum_radius = radiusA + radiusB;
	if( distance >= sum_radius ) 
	{
		return false;
	} 
	return true;
}
bool	DoDiscsContains(const Vec2& centerInner, float radiusInner, const Vec2& centerOuter, float radiusOuter)
{
	float distance = GetDistance2D(centerInner, centerOuter);
	if ((distance + radiusInner) < radiusOuter)
	{
		return true;
	}
	return false;
}
bool	DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB ) 
{
	float distance = GetDistance3D( centerA, centerB );
	float sum_radius = radiusA + radiusB;
	if( distance >= sum_radius )
	{
		return false;
	}
	return true;
}

bool	DoPolygonPlaneOverlap(const Polygon2D& polygon, const Plane2D& plane)
{
	Vec2 planeCenter = plane.distanceFromOriginAlongNormal * plane.normal;
	for (int vertexIndex = 0; vertexIndex < polygon.GetVertexCount(); ++vertexIndex)
	{
		if (DotProduct2D(polygon.GetPoints()[vertexIndex] - planeCenter, plane.normal) <= 0)
		{
			return true;
		}
	}
	return false;
}

bool	DoDiscPlaneOverlap(const Vec2& center, float radius, const Plane2D& plane)
{
	Vec2 planeCenter = plane.distanceFromOriginAlongNormal * plane.normal;
	if (DotProduct2D(center - planeCenter, plane.normal) <= radius)
	{
		return true;
	}
	return false;
}

void DoGiftWrapping(const std::vector<Vec2>& pointCloud, std::vector<Vec2>& resultPoints)
{
	if (pointCloud.size() <= 3)
	{
		resultPoints = pointCloud;
	}

	Vec2 leftMostPoint = pointCloud[0];
	for (int vertexIndex = 0; vertexIndex < pointCloud.size(); ++vertexIndex)
	{
		if (pointCloud[vertexIndex].x < leftMostPoint.x)
		{
			leftMostPoint = pointCloud[vertexIndex];
		}
	}
	resultPoints.push_back(leftMostPoint);
	
	Vec2 newPoint;
	Vec2 direction;
	if (newPoint == leftMostPoint)
	{
		newPoint = leftMostPoint +Vec2(0.5f, 0.f);
	}
	while (newPoint != leftMostPoint)
	{
		newPoint = pointCloud[0];
		if (resultPoints[resultPoints.size() - 1] == newPoint)
		{
			newPoint = pointCloud[1];
		}
		direction = newPoint - resultPoints[resultPoints.size() - 1];
		for (int vertexIndex = 1; vertexIndex < pointCloud.size(); ++vertexIndex)
		{
			if (CrossProduct2D((pointCloud[vertexIndex] - resultPoints[resultPoints.size() - 1]), direction) > 0.01f)
			{
				direction = pointCloud[vertexIndex] - resultPoints[resultPoints.size() - 1];
				newPoint = pointCloud[vertexIndex];
			}
		}
		resultPoints.push_back(newPoint);
	}
	resultPoints.pop_back();
}

const Vec2  TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	Vec2 toReturn = position * uniformScale;
	toReturn.RotateDegrees(rotationDegrees);
	toReturn += translation;
	return toReturn;
}

const Vec2  TransformPosition2D(const Vec2& position, const Vec2& xAxis, const Vec2& yAxis, const Vec2& translation)
{
	Vec2 toReturn = Vec2::ZERO;
	toReturn.x = xAxis.x * position.x + yAxis.x * position.y + translation.x;
	toReturn.y = xAxis.y * position.x + yAxis.y * position.y + translation.y;
	return toReturn;
}

const Vec3  TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY )
{
	Vec3 toReturn = position * scaleXY;
	toReturn = toReturn.GetRotatedAboutZDegrees(zRotationDegrees);
	toReturn += Vec3(translationXY, 0.f);
	toReturn.z = position.z;
	return toReturn;
}

const Vec3  TransformPosition3DXY(const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot)
{
	Vec3 toReturn = position * scaleXY;
	toReturn += Vec3(translationXYPreRot, 0.f);
	toReturn = toReturn.GetRotatedAboutZDegrees(zRotationDegrees);
	toReturn += Vec3(translationXYPostRot, 0.f);
	toReturn.z = position.z;
	return toReturn;
}

const Vec3  TransformPosition3DXY(const Vec3& position, const Vec2& xAxis, const Vec2& yAxis, const Vec2& translation)
{
	Vec3 toReturn = Vec3(0.f, 0.f, position.z);
	toReturn.x = xAxis.x * position.x + yAxis.x * position.y + translation.x;
	toReturn.y = xAxis.y * position.x + yAxis.y * position.y + translation.y;
	return toReturn;
}

void    TransformVertex( Vertex_PCU& vertex, float scaleXY, float zRotationDegrees, const Vec2& translationXY)
{
	Vec3 vertexPos = vertex.m_position;
	vertexPos = TransformPosition3DXY(vertexPos, scaleXY, zRotationDegrees, translationXY);
	vertex.m_position = vertexPos;
}

void    TransformVertex(Vertex_PCU& vertex, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot)
{

	Vec3 vertexPos = vertex.m_position;
	vertexPos = TransformPosition3DXY(vertexPos, scaleXY, zRotationDegrees, translationXYPostRot, translationXYPreRot);
	vertex.m_position = vertexPos;
}

void    TransformVertexArray(int numVerts, Vertex_PCU* vertices, float scaleXY, float zRotationDegrees, const Vec2& translationXY)
{
	for (int vertexIndex = 0; vertexIndex < numVerts; ++vertexIndex)
	{
		TransformVertex(vertices[vertexIndex], scaleXY, zRotationDegrees, translationXY);
	}
}

void	TransformVertexArray(int numVerts, Vertex_PCU* vertices, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot)
{
	for (int vertexIndex = 0; vertexIndex < numVerts; ++vertexIndex)
	{
		TransformVertex(vertices[vertexIndex], scaleXY, zRotationDegrees, translationXYPostRot, translationXYPreRot);
	}
}

void	AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, const AABB2& bounds, const Rgba8& tint, const Vec2& uvAtMins, const Vec2& uvAtMaxs)
{
	vertices.push_back(Vertex_PCU(Vec3(bounds.mins.x, bounds.mins.y, 0.f), tint, uvAtMins));
	vertices.push_back(Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(uvAtMaxs.x, uvAtMins.y)));
	vertices.push_back(Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(uvAtMins.x, uvAtMaxs.y)));
	vertices.push_back(Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(uvAtMins.x, uvAtMaxs.y)));
	vertices.push_back(Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(uvAtMaxs.x, uvAtMins.y)));
	vertices.push_back(Vertex_PCU(Vec3(bounds.maxs.x, bounds.maxs.y, 0.f), tint, uvAtMaxs));
}

void	AddVertsForSegment(std::vector<Vertex_PCU>& vertices, const Vec2& start, const Vec2& end, float thickness, Rgba8 color)
{
	float half_thickness = thickness * 0.5f;
	Vec2 forward = (end - start).GetNormalized() * half_thickness;
	Vec2 left = forward.GetRotated90Degrees();
	Vec2 endLeft = end + forward + left;
	Vec2 endRight = end + forward - left;
	Vec2 startLeft = start - forward + left;
	Vec2 startRight = start - forward - left;
	Rgba8 segColor = color;
	Vec2 segUV = Vec2();
	Vertex_PCU segVertices[] = {
		Vertex_PCU(Vec3(endLeft, 0.f), segColor, segUV),//triangle 1
		Vertex_PCU(Vec3(startRight, 0.f), segColor, segUV),
		Vertex_PCU(Vec3(endRight, 0.f), segColor, segUV),

		Vertex_PCU(Vec3(endLeft, 0.f), segColor, segUV),//triangle 2
		Vertex_PCU(Vec3(startLeft, 0.f), segColor, segUV),
		Vertex_PCU(Vec3(startRight, 0.f), segColor, segUV)
	};
	AddVerts(vertices, segVertices, 6);
}

void	AddVertsForDisc(std::vector<Vertex_PCU>& vertices, const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color)
{
	constexpr float deltaTheta = 360.f / 48.f;
	Vertex_PCU discVertices[144];
	const Vec2 asteroidUV = Vec2();
	Vertex_PCU point1 = Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, asteroidUV);
	for (int indexTriangle = 0; indexTriangle < 48; ++indexTriangle)
	{
		Vec3 pos2 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)indexTriangle, radius), 0.f);
		Vec3 pos3 = Vec3(Vec2::MakeFromPolarDegrees(deltaTheta * (float)(indexTriangle + 1), radius), 0.f);
		Vertex_PCU point2 = Vertex_PCU(pos2, color, asteroidUV);
		Vertex_PCU point3 = Vertex_PCU(pos3, color, asteroidUV);
		discVertices[indexTriangle * 3] = point1;
		discVertices[indexTriangle * 3 + 1] = point2;
		discVertices[indexTriangle * 3 + 2] = point3;
	}
	TransformVertexArray(144, discVertices, 1.f, orientationDegree, center, offset);
	AddVerts(vertices, discVertices, 144);
}

void	AddVertsForCircle(std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, float thickness, Rgba8 color)
{
	float half_thinkness = thickness * 0.5f;
	constexpr float deltaDegree = 360.f / 64.f;
	constexpr int numVertices = 64 * 2 * 3;
	float innerRadius = radius - half_thinkness;
	float outerRadius = radius + half_thinkness;
	Vertex_PCU circleVertices[numVertices];
	Vec2 circeUV = Vec2();
	for (int indexAngle = 0; indexAngle < 64; ++indexAngle)
	{
		Vec2 innerPosition1 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)indexAngle, innerRadius);
		Vec2 outerPosition1 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)indexAngle, outerRadius);
		Vertex_PCU innerVertex1 = Vertex_PCU(Vec3(innerPosition1, 0.f), color, circeUV);
		Vertex_PCU outerVertex1 = Vertex_PCU(Vec3(outerPosition1, 0.f), color, circeUV);

		Vec2 innerPosition2 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)(indexAngle + 1), innerRadius);
		Vec2 outerPosition2 = Vec2::MakeFromPolarDegrees(deltaDegree * (float)(indexAngle + 1), outerRadius);
		Vertex_PCU innerVertex2 = Vertex_PCU(Vec3(innerPosition2, 0.f), color, circeUV);
		Vertex_PCU outerVertex2 = Vertex_PCU(Vec3(outerPosition2, 0.f), color, circeUV);

		circleVertices[indexAngle * 6] = innerVertex1;//triangle1
		circleVertices[indexAngle * 6 + 1] = outerVertex1;
		circleVertices[indexAngle * 6 + 2] = innerVertex2;

		circleVertices[indexAngle * 6 + 3] = outerVertex1;//triangle2
		circleVertices[indexAngle * 6 + 4] = outerVertex2;
		circleVertices[indexAngle * 6 + 5] = innerVertex2;
	}
	TransformVertexArray(numVertices, circleVertices, 1.f, 0.f, center);
	AddVerts(vertices, circleVertices, numVertices);
}

void	AddVertsForPolygon2D(std::vector<Vertex_PCU>& vertices, const Polygon2D& polygon, const Rgba8& tint)
{
	Vec2 center = polygon.GetCenter();
	int numPoints = polygon.GetVertexCount();

	std::vector<Vec2> points = polygon.GetPoints();
	std::vector<Vertex_PCU> polyVertices;
	for (int edgeIndex = 0; edgeIndex < numPoints - 1; ++edgeIndex)
	{
		polyVertices.push_back(Vertex_PCU(Vec3(center, 0.f), tint, Vec2::ZERO));
		polyVertices.push_back(Vertex_PCU(Vec3(points[edgeIndex], 0.f), tint, Vec2::ZERO));
		polyVertices.push_back(Vertex_PCU(Vec3(points[edgeIndex + 1], 0.f), tint, Vec2::ZERO));
	}
	polyVertices.push_back(Vertex_PCU(Vec3(center, 0.f), tint, Vec2::ZERO));
	polyVertices.push_back(Vertex_PCU(Vec3(points[numPoints - 1], 0.f), tint, Vec2::ZERO));
	polyVertices.push_back(Vertex_PCU(Vec3(points[0], 0.f), tint, Vec2::ZERO));

	AddVerts(vertices, polyVertices);
}

void	AddVerts(std::vector<Vertex_PCU>& verticesFirst, std::vector<Vertex_PCU>& verticesSecond)
{
	verticesFirst.insert(verticesFirst.end(), verticesSecond.begin(), verticesSecond.end());
	verticesSecond.clear();
}

void	AddVerts(std::vector<Vertex_PCU>& verticesFirst, Vertex_PCU* verticesSecond, int length)
{
	for (int i = 0; i < length; ++i)
	{
		verticesFirst.push_back(verticesSecond[i]);
	}
}

float    Interpolate(float a, float b, float fractionOfB)
{
	return a + fractionOfB * (b - a);
}

Vec2   Interpolate(Vec2 const& a, Vec2 const& b, float fractionOfB)
{
	return a + fractionOfB * (b - a);
}

float   RangeMapFloat(float inBegin, float inEnd, float outBegin, float outEnd, float inValue)
{
	float inDisplacement = inValue - inBegin;
	float inRange = inEnd - inBegin;
	float fraction = inDisplacement / inRange;
	return Interpolate(outBegin, outEnd, fraction);
}

Vec2    RangeMapVec2(float inBegin, float inEnd, Vec2 const& outBegin, Vec2 const& outEnd, float inValue)
{
	float inDisplacement = inValue - inBegin;
	float inRange = inEnd - inBegin;
	float fraction = inDisplacement / inRange;
	return Interpolate(outBegin, outEnd, fraction);
}

float SignFloat(float val)
{
	return (val >= 0.0f) ? 1.0f : -1.0f;
}
float	Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	else {
		return value;
	}
}

int	ClampInt(int value, int min, int max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	else {
		return value;
	}
}

float	ClampZeroToOne(float value)
{
	return Clamp(value, 0.f, 1.f);
}

float	Round(float value)
{
	return roundf(value);
}

int		RoundDownToInt(float value)
{
	return static_cast<int>(floorf(value));
}

int		RoundToNearestInt(float value)
{
	return static_cast<int>(roundf(value));
}

float	DotProduct2D(const Vec2& vec1, const Vec2& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

float	DotProduct3D(const Vec3& vec1, const Vec3& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}
float	DotProduct4D(const Vec4& vec1, const Vec4& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
}

Vec2    TripleCrossProduct(const Vec2& vec1, const Vec2& vec2, const Vec2& vec3)
{
	Vec3 vec31 = Vec3(vec1, 0.f);
	Vec3 vec32 = Vec3(vec2, 0.f);
	Vec3 vec33 = Vec3(vec3, 0.f);
	Vec3 result3 = CrossProduct3D(CrossProduct3D(vec31, vec32), vec33);
	return Vec2(result3.x, result3.y);
}

float	CrossProduct2D(const Vec2& vec1, const Vec2& vec2)
{
	return (vec1.x * vec2.y) - (vec1.y * vec2.x);
}

Vec3    CrossProduct3D(const Vec3& vec1, const Vec3& vec2)
{
	float newX = vec1.y * vec2.z - vec1.z * vec2.y;
	float newY = vec1.z * vec2.x - vec1.x * vec2.z;
	float newZ = vec1.x * vec2.y - vec1.y * vec2.x;
	return Vec3(newX, newY, newZ);
}

Vec3 SphericalToCartesian(float theta, float psi, float radius)
{
	float x = CosDegrees(psi) * CosDegrees(theta);
	float y = SinDegrees(psi);
	float z = CosDegrees(psi) * SinDegrees(theta);
	return radius * Vec3(x, y, z);
}

const Vec2 GetNearestPointOnDisc2D(const Vec2& point, const Vec2& center, float radius)
{
	if (GetDistance2D(center, point) < radius)
	{
		return point;
	}
	else
	{
		return center + Vec2::MakeFromPolarDegrees((point - center).GetAngleDegrees(), radius);
	}
}

const Vec2 GetNearestPointOnAABB2D(const Vec2& point, const AABB2& box)
{
	return box.GetNearestPoint(point);
}

const Vec2 GetNearestPointOnInfiniteLine2D(const Vec2& point, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine)
{
	Vec2 normal = (anotherPointOnLine - somePointOnLine).GetNormalized();
	Vec2 localPoint = point - somePointOnLine;
	Vec2 projectedLocalPoint = GetProjectedOnto2D(localPoint, normal);
	return projectedLocalPoint + somePointOnLine;
}

const Vec2 GetNearestPointOnRay2D(const Vec2& point, const Vec2& start, const Vec2& end)
{
	if (GetDistance2D(start, end) < 0.01f)
	{
		return start;
	}
	Vec2 normal = (end - start).GetNormalized();
	Vec2 localPoint = point - start;
	float projectedLength = GetProjectedLength2D(localPoint, normal);
	if (projectedLength <= 0.f)
	{
		return start;
	}
	else
	{
		Vec2 localProjectedPoint = projectedLength * normal;
		return localProjectedPoint + start;
	}
}

const Vec2 GetNearestPointOnLineSegment2D(const Vec2& point, const Vec2& start, const Vec2& end)
{
	if (GetDistance2D(start, end) < 0.01f)
	{
		return start;
	}
	Vec2 normal = (end - start).GetNormalized();
	Vec2 localPoint = point - start;
	float projectedLength = GetProjectedLength2D(localPoint, normal);
	float length = GetDistance2D(start, end);
	if (projectedLength <= 0.f)
	{
		return start;
	}
	else if (projectedLength >= length)
	{
		return end;
	}
	else
	{
		Vec2 localProjectedPoint = projectedLength * normal;
		return localProjectedPoint + start;
	}

}

const Vec2 GetNearestPointOnCapsule2D(const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius)
{
	if (IsPointInsideCapsule2D(point, capsuleMidStart, capsuleMidEnd, capsuleRadius))
	{
		return point;
	}
	else
	{
		Vec2 nearestPointOnSegment = GetNearestPointOnLineSegment2D(point, capsuleMidStart, capsuleMidEnd);
		Vec2 normal = (point - nearestPointOnSegment).GetNormalized();
		return nearestPointOnSegment + capsuleRadius * normal;
	}
}
const Vec2 GetNearestPointOnOBB2D(const Vec2& point, const OBB2& box)
{
	return box.GetNearestPoint(point);
}

const Vec2 GetNearestPointOnPolygon2D(const Vec2& point, std::vector<Vec2> const& polygon)
{
	if (IsPointInsidePolygon2D(point, polygon))
	{
		return point;
	}
	return GetNearestPointOnPolygon2DEdge(point, polygon);
}

const Vec2 GetNearestPointOnPolygon2DEdge(const Vec2& point, std::vector<Vec2> const& polygon)
{
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D(point, polygon[0], polygon[polygon.size() - 1]);
	float shortestDistance = GetDistance2D(nearestPoint, point);
	for (int vertexIndex = 0; vertexIndex < polygon.size() - 1; ++vertexIndex)
	{
		Vec2 currPoint = GetNearestPointOnLineSegment2D(point, polygon[vertexIndex], polygon[vertexIndex + 1]);
		float distance = GetDistance2D(currPoint, point);
		if (distance < shortestDistance)
		{
			shortestDistance = distance;
			nearestPoint = currPoint;
		}
	}
	return nearestPoint;
}

void	GetIntersectPointsBetweenSegementsAndDisc(const Vec2& start, const Vec2& end, const Vec2& center, float radius, Vec2& pointA, Vec2& pointB)
{
	//Move center to origin
	Vec2 newStart = start - center;
	Vec2 newEnd = end - center;
	Vec2 segment = newEnd - newStart;
	float degrees = segment.GetAngleDegrees();
	newStart.RotateDegrees(-degrees);
	//Move To New Space Done

	//find tQ and points
	if ((radius * radius) < (newStart.y * newStart.y))
	{
		return;
	}
	float tQ = (float)sqrt((radius * radius) - (newStart.y * newStart.y)) - newStart.x;
	pointA = Vec2(newStart.x + tQ, newStart.y);
	pointB = Vec2(-pointA.x, newStart.y);
	pointA.RotateDegrees(degrees);
	pointA = pointA + center;
	pointB.RotateDegrees(degrees);
	pointB = pointB + center;
}

bool IsPointInsideDisk2D(const Vec2& point, const Vec2& discCenter, float discRadius)
{
	float distance = GetDistance2D(point, discCenter);
	if (distance <= discRadius + 0.0001f)
	{
		return true;
	}
	return false;
}

bool IsPointInsideAABB2D(const Vec2& point, const AABB2& box)
{
	return box.IsPointInside(point);
}

bool IsPointInsideCapsule2D(const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius)
{
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D(point, capsuleMidStart, capsuleMidEnd);
	float distance = GetDistance2D(point, nearestPoint);
	if (distance <= capsuleRadius)
	{
		return true;
	}
	return false;
}

bool IsPointInsideOBB2D(const Vec2& point, const OBB2& box)
{
	return box.IsPointInside(point);
}

bool IsPointInsidePolygon2D(const Vec2& point, std::vector<Vec2> const& polygon)
{
	int numVertices = (int)polygon.size();
	for (int vertexIndex = 0; vertexIndex < polygon.size(); ++vertexIndex)
	{
		float crossProduct = CrossProduct2D(polygon[(vertexIndex + 1) % numVertices] - polygon[vertexIndex % numVertices],
			point - polygon[vertexIndex % numVertices]);
		if (crossProduct <= 0)
		{
			return false;
		}
	}
	return true;
}

bool    IsOriginInsideTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC)
{
	Vec2 AtoOrigin = -pointA;
	Vec2 BToOrigin = -pointB;
	Vec2 AtoB = pointB - pointA;
	Vec2 AtoC = pointC - pointA;
	Vec2 BtoC = pointC - pointB;
	Vec2 ABPerp = TripleCrossProduct(AtoC, AtoB, AtoB);
	Vec2 ACPerp = TripleCrossProduct(AtoB, AtoC, AtoC);
	Vec2 BCPerp = TripleCrossProduct(-AtoB, BtoC, BtoC);
	if (DotProduct2D(ABPerp, AtoOrigin) > 0.f)
	{
		return false;
	}
	if (DotProduct2D(ACPerp, AtoOrigin) > 0.f)
	{
		return false;
	}
	if (DotProduct2D(BCPerp, BToOrigin) > 0.f)
	{
		return false;
	}
	return true;
}

void	PushDiscOutOfDisc2D(Vec2& centerMobile, float radiusMobile, const Vec2& centerFixed, float radiusFixed)
{
	float overlapDist = radiusFixed + radiusMobile - GetDistance2D(centerFixed, centerMobile);
	if (overlapDist <= 0.f)
	{
		return;
	}
	Vec2 diff = centerMobile - centerFixed;
	diff.Normalize();
	Vec2 Correction = overlapDist * diff;
	centerMobile += Correction;
}

void	PushDiscsOutOfEachOther2D(Vec2& centerA, float radiusA, Vec2& centerB, float radiusB, float massRatioA)
{
	float overlapDist = radiusA + radiusB - GetDistance2D(centerA, centerB);
	if (overlapDist <= 0.f)
	{
		return;
	}
	Vec2 diff = centerB - centerA;
	diff.Normalize();
	
	Vec2 BCorrection = overlapDist * massRatioA * diff;
	Vec2 ACorrection = -overlapDist * (1.f - massRatioA) * diff;
	centerB += BCorrection;
	centerA += ACorrection;
}

void	PushDiscOutOfPoint2D(Vec2& centerMobile, float radiusMobile, const Vec2& pointFixed)
{
	float overlapDist = radiusMobile - GetDistance2D(pointFixed, centerMobile);
	if (overlapDist <= 0.f)
	{
		return;
	}
	Vec2 diff = centerMobile - pointFixed;
	diff.Normalize();
	Vec2 Correction = overlapDist * diff;
	centerMobile += Correction;
}

void	PushDiscOutOfAABB2D(Vec2& center, float radius, const AABB2& aabb)
{
	Vec2 nearestPoint = aabb.GetNearestPoint(center);
	PushDiscOutOfPoint2D(center, radius, nearestPoint);
}

void	PushDiscAndAABB2DOutOfEachOther(Vec2& center, float radius, const AABB2& aabb, Vec2& aabbCenter)
{
	Vec2 nearestPoint = aabb.GetNearestPoint(center);
	Vec2 pointBefore = nearestPoint;
	PushDiscsOutOfEachOther2D(center, radius, nearestPoint, 0.01f);
	aabbCenter += nearestPoint - pointBefore;
}

const Vec2 GetProjectedOnto2D(const Vec2& toBeProjected, const Vec2& toBeProjectedOn)
{
	float length = GetProjectedLength2D(toBeProjected, toBeProjectedOn);
	return length * toBeProjectedOn.GetNormalized();
}

const float GetProjectedLength2D(const Vec2& toBeProjected, const Vec2& toBeProjectedOn)
{
	return DotProduct2D(toBeProjected, toBeProjectedOn) / toBeProjectedOn.GetLength();
}

const float GetAngleDegreesBetweenVectors2D(const Vec2& vecA, const Vec2& vecB)
{
	float cos = DotProduct2D(vecA.GetNormalized(), vecB.GetNormalized());
	if (cos > 1.f)
	{
		cos = 1.f;
	}
	if (cos < -1.f)
	{
		cos = -1.f;
	}
	return ACosDegrees(cos);
}

const int GetTaxicabDistance2D(IntVec2 vecA, IntVec2 vecB)
{
	int disX = abs(vecA.x - vecB.x);
	int disY = abs(vecA.y - vecB.y);
	return disX + disY;
}

bool IsPolygon2DConvex(std::vector<Vec2> const& polygon)
{
	int numVertices = (int)polygon.size();
	for (int vertexIndex = 0; vertexIndex < polygon.size(); ++vertexIndex)
	{
		float crossProduct = CrossProduct2D(polygon[(vertexIndex + 1) % numVertices] - polygon[vertexIndex % numVertices],
			polygon[(vertexIndex + 2) % numVertices] - polygon[(vertexIndex + 1) % numVertices]);
		if (crossProduct < 0)
		{
			return false;
		}
	}
	return true;
}

float GetApproximateRadiusOfPolygon2D(Polygon2D const& polygon)
{
	float farthestDistance = 0;
	Vec2 center = polygon.GetCenter();
	std::vector<Vec2> points = polygon.GetPoints();
	int numPoints = polygon.GetVertexCount();
	for (int vertexIndex = 0; vertexIndex < numPoints; ++vertexIndex)
	{
		float distance = GetDistance2D(center, points[vertexIndex]);
		if (distance > farthestDistance)
		{
			farthestDistance = distance;
		}
	}
	return farthestDistance;
}

Vec2 GetAveragePoint2D(std::vector<Vec2> const& polygon)
{
	Vec2 sumVertex = Vec2::ZERO;
	for (int vertexIndex = 0; vertexIndex < polygon.size(); ++vertexIndex)
	{
		sumVertex += polygon[vertexIndex];
	}
	return sumVertex / (float)polygon.size();
}

Vec2 GetIntersectOfTwoLine(const Vec2& startA, const Vec2& endA, const Vec2& startB, const Vec2& endB)
{
	float intersectX = 0.f;
	float intersectY = 0.f;
	//remember to handle vertical line
	if (abs(endA.x - startA.x) < 0.0001f)
	{
		intersectX = startA.x;
		float slopeB = (endB.y - startB.y) / (endB.x - startB.x);
		float interceptB = startB.y - slopeB * startB.x;
		intersectY = slopeB * intersectX + interceptB;
	}
	else if (abs(endB.x - startB.x) < 0.0001f)
	{
		intersectX = startB.x;
		float slopeA = (endA.y - startA.y) / (endA.x - startA.x);
		float interceptA = startA.y - slopeA * startA.x;
		intersectY = slopeA * intersectX + interceptA;
	}
	else
	{
		float slopeA = (endA.y - startA.y) / (endA.x - startA.x);
		float slopeB = (endB.y - startB.y) / (endB.x - startB.x);
		float interceptA = startA.y - slopeA * startA.x;
		float interceptB = startB.y - slopeB * startB.x;

		intersectX = (interceptB - interceptA) / (slopeA - slopeB);
		intersectY = slopeA * intersectX + interceptA;
	}
	return Vec2(intersectX, intersectY);
}

Disc2 GetTriangleCircumscribedCircle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC)
{
	Vec2 startA = (pointA + pointB) * 0.5f;
	Vec2 startB = (pointB + pointC) * 0.5f;

	Vec2 endA = startA + (pointB - pointA).GetNormalized().GetRotated90Degrees();
	Vec2 endB = startB + (pointC - pointB).GetNormalized().GetRotated90Degrees();

	Vec2 center = GetIntersectOfTwoLine(startA, endA, startB, endB);
	float radius = GetDistance2D(center, pointA);
	Disc2 disc;
	disc.m_center = center;
	disc.m_radius = radius;
	return disc;
}

Disc2 GetDiscFromTwoPoints(const Vec2& pointA, const Vec2& pointB)
{
	Vec2 center = (pointA + pointB) * 0.5f;
	float radius = GetDistance2D(center, pointA);
	Disc2 disc;
	disc.m_center = center;
	disc.m_radius = radius;
	return disc;
}

Disc2 GetTightBoundDiscOfPolygon(std::vector<Vec2> const& polygon)
{
	float bestRadius = 999999.f;
	Vec2 bestCenter = Vec2::ZERO;
	for (int vertexIndex = 0; vertexIndex < polygon.size(); ++vertexIndex)
	{
		for (int vertexIndexJ = vertexIndex + 1; vertexIndexJ < polygon.size(); ++vertexIndexJ)
		{
			Disc2 disc = GetDiscFromTwoPoints(polygon[vertexIndex], polygon[vertexIndexJ]);
			bool containsAll = true;
			for (int i = 0; i < polygon.size(); ++i)
			{
				if (!IsPointInsideDisk2D(polygon[i], disc.m_center, disc.m_radius))
				{
					containsAll = false;
					break;
				}
			}
			if (containsAll && disc.m_radius < bestRadius)
			{
				bestRadius = disc.m_radius;
				bestCenter = disc.m_center;
			}
		}
	}

	for (int vertexIndex = 0; vertexIndex < polygon.size(); ++vertexIndex)
	{
		for (int vertexIndexJ = vertexIndex + 1; vertexIndexJ < polygon.size(); ++vertexIndexJ)
		{
			for (int vertexIndexK = vertexIndexJ + 1; vertexIndexK < polygon.size(); ++vertexIndexK)
			{
				Disc2 disc = GetTriangleCircumscribedCircle(polygon[vertexIndex], polygon[vertexIndexJ], polygon[vertexIndexK]);
				bool containsAll = true;
				for (int i = 0; i < polygon.size(); ++i)
				{
					if (!IsPointInsideDisk2D(polygon[i], disc.m_center, disc.m_radius))
					{
						containsAll = false;
						break;
					}
				}
				if (containsAll && disc.m_radius < bestRadius)
				{
					bestRadius = disc.m_radius;
					bestCenter = disc.m_center;
				}
			}
		}
	}

	Disc2 bestDisc;
	bestDisc.m_radius = bestRadius;
	bestDisc.m_center = bestCenter;
	return bestDisc;
}

Vec2 GetCenterOfMassOfPolygon(std::vector<Vec2> const& polygon)
{
	Vec2 startPoint = polygon[0];
	std::vector<Vec2> centers;
	std::vector<float> areas;
	float polyArea = 0.f;
	for (int i = 1; i < polygon.size() - 1; ++i)
	{
		for (int j = i + 1; j < polygon.size(); ++j)
		{
			Vec2 center = GetTriangleCircumscribedCircle(startPoint, polygon[i], polygon[j]).m_center;
			centers.push_back(center);
			float area = GetAreaOfTriangle(startPoint, polygon[i], polygon[j]);
			areas.push_back(area);
			polyArea += area;
		}
	}
	Vec2 polyCenter = Vec2::ZERO;
	for (int i = 0; i < centers.size(); ++i)
	{
		polyCenter += centers[i] * (areas[i] / polyArea);
	}
	return polyCenter;
}

Vec2 GetSupportOfPolygon(std::vector<Vec2> const& polygon, const Vec2& direction)
{
	float maxDistance = DotProduct2D(polygon[0], direction);
	Vec2 supportPoint = polygon[0];
	for (int verticesIndex = 1; verticesIndex < polygon.size(); ++verticesIndex)
	{
		float distance = DotProduct2D(polygon[verticesIndex], direction);
		if (distance > maxDistance)
		{
			maxDistance = distance;
			supportPoint = polygon[verticesIndex];
		}
	}
	return supportPoint;
}

bool IsPointPassedOrigin(const Vec2& point, const Vec2& direction)
{
	return DotProduct2D(point, direction) > 0.f;
}

Vec2 GetPointOfTriangleNotOnClosestEdgeOfPoint(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC, const Vec2& point)
{
	Vec2 AB = pointB - pointA;
	Vec2 BC = pointC - pointB;
	Vec2 CA = pointA - pointC;

	if (signbit(CrossProduct2D(AB, BC)) != signbit(CrossProduct2D(point + pointB, BC)))
	{
		return pointA;
	}
	if (signbit(CrossProduct2D(BC, CA)) != signbit(CrossProduct2D(point + pointC, CA)))
	{
		return pointB;
	}
	else return pointC;
}

Segment2D ClipSegmentToSegment(Segment2D const& toClip, Segment2D const& refEdge)
{
	Vec2 Ci = toClip.pointA;
	Vec2 Cf = toClip.pointB;

	Vec2 Ri = refEdge.pointA;
	Vec2 Rf = refEdge.pointB;

	Vec2 Rd = (Rf - Ri).GetNormalized();
	float Pri = DotProduct2D(Ri, Rd);
	float Prf = DotProduct2D(Rf, Rd);
	float Pci = DotProduct2D(Ci, Rd);
	float Pcf = DotProduct2D(Cf, Rd);

	float fi = Pri;
	if (Pci > fi)
	{
		fi = Pci;
	}

	float ff = Prf;
	if (Pcf < ff)
	{
		ff = Pcf;
	}

	Vec2 Fi = RangeMapVec2(Pci, Pcf, Ci, Cf, fi);
	Vec2 Ff = RangeMapVec2(Pci, Pcf, Ci, Cf, ff);
	return Segment2D(Fi, Ff);
}

bool IsPolygonIntersectWithOtherPolygon(Polygon2D const& polygonA, Polygon2D const& polygonB)
{
	Vec2 direction = polygonB.GetCenter() - polygonA.GetCenter();
	Vec2 vert1 = polygonA.GetSupport(direction) - polygonB.GetSupport(-direction);
	Vec2 vert2 = polygonA.GetSupport(-direction) - polygonB.GetSupport(direction);
	Vec2 vert1ToVert2 = (vert2 - vert1).GetNormalized();
	Vec2 vert1ToOrigin = (-vert1).GetNormalized();
	Vec2 normalDirection = TripleCrossProduct((vert1 - vert2), -vert2, (vert1 - vert2));
	Vec2 vert3 = polygonA.GetSupport(normalDirection) - polygonB.GetSupport(-normalDirection);
	
	//Make simplex(polygon)
	std::vector<Vec2> vertices;
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);
	while (!IsOriginInsideTriangle(vertices[0], vertices[1], vertices[2]))
	{
		Vec2 vertA = vertices[0];
		Vec2 vertB = vertices[1];
		Vec2 vertC = vertices[2];
		
		Vec2 pointNotOnNearestEdge =
			GetPointOfTriangleNotOnClosestEdgeOfPoint(vertA, vertB, vertC, Vec2::ZERO);
		
		if (pointNotOnNearestEdge == vertA)
		{
			vertices.erase(vertices.begin());
		}
		else if (pointNotOnNearestEdge == vertB)
		{
			vertices.erase(vertices.begin() + 1);
		}
		else
		{
			vertices.erase(vertices.begin() + 2);
		}

		Vec2 nearestEdge = vertices[1] - vertices[0];
		if (CrossProduct2D(nearestEdge, -vertices[0]) < 0)
		{
			direction = nearestEdge.GetRotatedMinus90Degrees().GetNormalized();
		}
		else
		{
			direction = nearestEdge.GetRotated90Degrees().GetNormalized();
		}
		Vec2 newVert = polygonA.GetSupport(direction) - polygonB.GetSupport(-direction);
		if (!IsPointPassedOrigin(newVert, direction))
		{
			return false;
		}
		vertices.push_back(newVert);
	}
	return true;
}

Vec2 GetContactNormalOfTwoPolygons(Polygon2D const& polygonA, Polygon2D const& polygonB)
{
	Vec2 direction = polygonB.GetCenter() - polygonA.GetCenter();
	Vec2 vert1 = polygonA.GetSupport(direction) - polygonB.GetSupport(-direction);
	Vec2 vert2 = polygonA.GetSupport(-direction) - polygonB.GetSupport(direction);
	Vec2 vert1ToVert2 = (vert2 - vert1).GetNormalized();
	Vec2 vert1ToOrigin = (-vert1).GetNormalized();
	Vec2 normalDirection = TripleCrossProduct((vert1 - vert2), -vert2, (vert1 - vert2));
	Vec2 vert3 = polygonA.GetSupport(normalDirection) - polygonB.GetSupport(-normalDirection);

	//Make simplex(polygon)
	std::vector<Vec2> vertices;
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);
	while (!IsOriginInsideTriangle(vertices[0], vertices[1], vertices[2]))
	{
		Vec2 vertA = vertices[0];
		Vec2 vertB = vertices[1];
		Vec2 vertC = vertices[2];

		Vec2 pointNotOnNearestEdge =
			GetPointOfTriangleNotOnClosestEdgeOfPoint(vertA, vertB, vertC, Vec2::ZERO);

		if (pointNotOnNearestEdge == vertA)
		{
			vertices.erase(vertices.begin());
		}
		else if (pointNotOnNearestEdge == vertB)
		{
			vertices.erase(vertices.begin() + 1);
		}
		else
		{
			vertices.erase(vertices.begin() + 2);
		}

		Vec2 nearestEdge = vertices[1] - vertices[0];
		if (CrossProduct2D(nearestEdge, -vertices[0]) < 0)
		{
			direction = nearestEdge.GetRotatedMinus90Degrees().GetNormalized();
		}
		else
		{
			direction = nearestEdge.GetRotated90Degrees().GetNormalized();
		}
		Vec2 newVert = polygonA.GetSupport(direction) - polygonB.GetSupport(-direction);
		if (!IsPointPassedOrigin(newVert, direction))
		{
			return Vec2::ZERO;//no contact point
		}
		vertices.push_back(newVert);
	}
	//Make sure triangle is counter clockwise
	if (!IsPolygon2DConvex(vertices))
	{
		vertices.push_back(vertices[1]);
		vertices.erase(vertices.begin() + 1);
	}
	while (1)
	{
		Vec2 nearestPoint = GetNearestPointOnLineSegment2D(Vec2::ZERO, vertices[vertices.size() - 1], vertices[0]);
		float shortestDistance = GetDistance2D(nearestPoint, Vec2::ZERO);
		int shortestEdgeIndex = 0;
		for (int vertexIndex = 0; vertexIndex < vertices.size() - 1; ++vertexIndex)
		{
			Vec2 currPoint = GetNearestPointOnLineSegment2D(Vec2::ZERO, vertices[vertexIndex], vertices[vertexIndex + 1]);
			float distance = GetDistance2D(currPoint, Vec2::ZERO);
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
				nearestPoint = currPoint;
				shortestEdgeIndex = vertexIndex + 1;
			}
		}
		//get direction and new vert
		
		Vec2 splitDirection = nearestPoint.GetNormalized();
		Vec2 newVert = polygonA.GetSupport(splitDirection) - polygonB.GetSupport(-splitDirection);
		int startIndex = shortestEdgeIndex - 1;
		if (startIndex == -1)
		{
			startIndex = (int)vertices.size() - 1;
		}
		Vec2 edgeStart = vertices[startIndex];
		Vec2 edgeEnd = vertices[shortestEdgeIndex];

		//If cannot split, normal is found
		if (GetDistance2D(newVert, edgeStart) < 0.01f ||
			GetDistance2D(newVert, edgeEnd) < 0.01f)
		{
			return splitDirection * shortestDistance;
		}
		vertices.insert(vertices.begin() + shortestEdgeIndex, newVert);
	}
}

float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	return t * t * t;
}

float SmoothStart4(float t)
{
	return t * t * t * t;
}

float SmoothStart5(float t)
{
	return t * t * t * t * t;
}

float SmoothStart6(float t)
{
	return t * t * t * t * t * t;
}

float SmoothStop2(float t)
{
	return 1.f - (1.f - t) * (1.f - t);
}

float SmoothStop3(float t)
{
	return 1.f - (1.f - t) * (1.f - t) * (1.f - t);
}

float SmoothStop4(float t)
{
	return 1.f - (1.f - t) * (1.f - t) * (1.f - t) * (1.f - t);
}
float SmoothStop5(float t)
{
	return 1.f - (1.f - t) * (1.f - t) * (1.f - t) * (1.f - t) * (1.f - t);
}

float SmoothStep3(float t)
{
	return 3.f * (t * t) - 2.f * (t * t * t);
}

FloatRange GetRangeOnProjectedAxis(int numPoints, const Vec2* points, const Vec2& relativePos, const Vec2& axisNormal)
{
	float min = 9999999.f;
	float max = -9999999.f;
	for (int pointIndex = 0; pointIndex < numPoints; ++pointIndex)
	{
		float pointPosOnLine = GetProjectedLength2D(points[pointIndex] - relativePos, axisNormal);
		if (pointPosOnLine < min)
		{
			min = pointPosOnLine;
		}

		if (pointPosOnLine > max)
		{
			max = pointPosOnLine;
		}
	}
	return FloatRange(min, max);
}

bool DoOBBAndOBBOverlap2D(const OBB2& boxA, const OBB2& boxB)
{
	Vec2 IA = boxA.GetIBasisNormal();
	Vec2 JA = boxA.GetJBasisNormal();
	Vec2 IB = boxB.GetIBasisNormal();
	Vec2 JB = boxB.GetJBasisNormal();

	Vec2 dimensionsA = boxA.GetDimensions();
	Vec2 dimensionsB = boxB.GetDimensions();

	Vec2 boxAPoints[4];
	Vec2 boxBPoints[4];
	
	boxA.GetCornerPositions(boxAPoints);
	boxB.GetCornerPositions(boxBPoints);

	//check on IA
	FloatRange boxAOnIA = FloatRange(-dimensionsA.x * 0.5f, dimensionsA.x * 0.5f);
	FloatRange boxBOnIA = GetRangeOnProjectedAxis(4, boxBPoints, boxA.GetCenter(), IA);
	if (!boxAOnIA.DoesOverlap(boxBOnIA))
	{
		return false;
	}

	//check on JA
	FloatRange boxAOnJA = FloatRange(-dimensionsA.y * 0.5f, dimensionsA.y * 0.5f);
	FloatRange boxBOnJA = GetRangeOnProjectedAxis(4, boxBPoints, boxA.GetCenter(), JA);
	if (!boxAOnJA.DoesOverlap(boxBOnJA))
	{
		return false;
	}

	//check on IB
	FloatRange boxAOnIB = GetRangeOnProjectedAxis(4, boxAPoints, boxB.GetCenter(), IB);
	FloatRange boxBOnIB = FloatRange(-dimensionsB.x * 0.5f, dimensionsB.x * 0.5f);
	if (!boxAOnIB.DoesOverlap(boxBOnIB))
	{
		return false;
	}

	//check on JB
	FloatRange boxAOnJB = GetRangeOnProjectedAxis(4, boxAPoints, boxB.GetCenter(), JB);
	FloatRange boxBOnJB = FloatRange(-dimensionsB.y * 0.5f, dimensionsB.y * 0.5f);
	if (!boxAOnJB.DoesOverlap(boxBOnJB))
	{
		return false;
	}

	return true;
}

Vec3 SinCosPlane(float x, float z)
{
	return Vec3(x, (float)sin((double)x * 100.0) + (float)cos((double)z * 100.0), z);
}

float GetAreaOfTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC)
{
	float b = GetDistance2D(pointB, pointC);
	float h = GetDistance2D(pointA, GetNearestPointOnInfiniteLine2D(pointA, pointB, pointC));
	return b * h * 0.5f;
}

float GetMomentOfTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC)
{
	float b = GetDistance2D(pointB, pointC);
	float h = GetDistance2D(pointA, GetNearestPointOnInfiniteLine2D(pointA, pointB, pointC));
	Vec2 BA = pointA - pointB;
	Vec2 BCNormalized = (pointC - pointB).GetNormalized();
	float a = GetProjectedLength2D(BA, BCNormalized);
	return h * b * (b * b - b * a + a * a + h * h) * 0.0278f;
}

int FloatToPercent(float value)
{
	return (int)floor(value * 100.f);
}

float InfluenceFalloffExpotential(float startValue, float distance, float propagationConstant)
{
	return startValue * (float)pow(propagationConstant, distance);
}

float InfluenceFalloffLinear(float startValue, float distance, float maxDistance)
{
	return RangeMapFloat(0.f, maxDistance, startValue, 0.f, distance);
}

float InfluenceFalloffSquare(float startValue, float distance, float maxDistance)
{
	float linearFalloff = InfluenceFalloffLinear(startValue, distance, maxDistance);
	return linearFalloff * linearFalloff;
}

float GetPersonalInterestValuePower4(float oneOverRadius, int distance)
{
	float score = 1.f - (float)pow((oneOverRadius * (float)distance), 4.f);
	return Clamp(score, 0.001f, 1.f);
}

float GetPersonalInterestValuePower2(float oneOverRadius, int distance)
{
	float score = 1.f - (float)pow((oneOverRadius * (float)distance), 2.f);
	return Clamp(score, 0.001f, 1.f);
}

float GetPersonalInterestValueLinear(float oneOverRadius, int distance)
{
	float score = 1.f - oneOverRadius * (float)distance;
	return Clamp(score, 0.001f, 1.f);
}

int CheckIntPositivity(int number)
{
	if (number > 0)
	{
		return 1;
	}
	if (number < 0)
	{
		return -1;
	}
	return 0;
}

IntVec2 FloorVec2ToIntVec2(Vec2 vec)
{
	int x = RoundDownToInt(vec.x);
	int y = RoundDownToInt(vec.y);
	return IntVec2(x, y);
}