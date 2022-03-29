#pragma once
#include <math.h>
#include <vector>
struct Vec2;
struct IntVec2;
struct Vec3;
struct Vec4;
struct Vertex_PCU;
struct AABB2;
struct OBB2;
struct Rgba8;
struct FloatRange;
class Polygon2D;
struct Plane2D;
struct Disc2;
struct Segment2D;

float	ConvertDegreesToRadians( float degrees );
float	ConvertRadiansToDegrees( float radians );
float	CosDegrees( float degrees );
float   SinDegrees( float degrees );
float	ACosDegrees(float cos);
float   ASinDegrees(float sin);
float	Atan2Degrees( float y, float x );
float   GetShortestAngularDisplacement(float fromDegrees, float toDegrees);
float	GetTurnedToward(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
bool	IsPointInForwardSector2D(const Vec2& point, const Vec2& start, float maxDist, const Vec2& fwdDir, float apertureDegrees);
bool	IsPointInForwardSector2D(const Vec2& point, const Vec2& start, float fwdDegrees, float apertureDegrees, float maxDist);
float   GetDistance2D( const Vec2& positionA, const Vec2& positionB );
float	GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB );
float   GetDistance3D( const Vec3& positionA, const Vec3& positionB );
float	GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB );
float   GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB );
float	GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB );

bool	DoDiscsOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
bool	DoDiscsContains(const Vec2& centerInner, float radiusInner, const Vec2& centerOuter, float radiusOuter);
bool	DoSpheresOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);

bool	DoPolygonPlaneOverlap(const Polygon2D& polygon, const Plane2D& plane);
bool	DoDiscPlaneOverlap(const Vec2& center, float radius, const Plane2D& plane);

void DoGiftWrapping(const std::vector<Vec2>& pointCloud, std::vector<Vec2>& resultPoints);

const Vec2  TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation );
const Vec2  TransformPosition2D(const Vec2& position, const Vec2& xAxis, const Vec2& yAxis, const Vec2& translation);
const Vec3  TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY );
const Vec3  TransformPosition3DXY( const Vec3& position, const Vec2& xAxis, const Vec2& yAxis, const Vec2& translation );
const Vec3  TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot );

void    TransformVertex( Vertex_PCU& vertex, float scaleXY, float zRotationDegrees, const Vec2& translationXY );
void    TransformVertex(Vertex_PCU& vertex, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot);
void	TransformVertexArray(int numVerts, Vertex_PCU* vertices, float scaleXY, float zRotationDegrees, const Vec2& translationXY);
void	TransformVertexArray(int numVerts, Vertex_PCU* vertices, float scaleXY, float zRotationDegrees, const Vec2& translationXYPostRot, const Vec2& translationXYPreRot);

void	AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, const AABB2& bounds, const Rgba8& tint, const Vec2& uvAtMins, const Vec2& uvAtMaxs);
void	AddVertsForSegment(std::vector<Vertex_PCU>& vertices, const Vec2& start, const Vec2& end, float thickness, Rgba8 color);
void	AddVertsForDisc(std::vector<Vertex_PCU>& vertices, const Vec2& center, const Vec2& offset, float radius, float orientationDegree, Rgba8 color);
void	AddVertsForCircle(std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, float thickness, Rgba8 color);
void	AddVertsForPolygon2D(std::vector<Vertex_PCU>& vertices, const Polygon2D& polygon, const Rgba8& tint);
void	AddVerts(std::vector<Vertex_PCU>& verticesFirst, std::vector<Vertex_PCU>& verticesSecond);
void	AddVerts(std::vector<Vertex_PCU>& verticesFirst, Vertex_PCU* verticesSecond, int length);

float   Interpolate( float a, float b, float fractionOfB );
Vec2    Interpolate(Vec2 const& a, Vec2 const& b, float fractionOfB);
float   RangeMapFloat(float inBegin, float inEnd, float outBegin, float outEnd, float inValue);
Vec2    RangeMapVec2(float inBegin, float inEnd, Vec2 const& outBegin, Vec2 const& outEnd, float inValue);
float	SignFloat(float val);

float	Clamp(float value, float min, float max);
int		ClampInt(int value, int min, int max);
float	ClampZeroToOne(float value);
float	Round(float value);
int		RoundDownToInt(float value);
int		RoundToNearestInt(float value);
float	DotProduct2D(const Vec2& vec1, const Vec2& vec2);
float	DotProduct3D(const Vec3& vec1, const Vec3& vec2);
float	DotProduct4D(const Vec4& vec1, const Vec4& vec2);

Vec2    TripleCrossProduct(const Vec2& vec1, const Vec2& vec2, const Vec2& vec3);

float	CrossProduct2D(const Vec2& vec1, const Vec2& vec2);
Vec3    CrossProduct3D(const Vec3& vec1, const Vec3& vec2);

Vec3 SphericalToCartesian(float theta, float psi, float radius);

const Vec2 GetNearestPointOnDisc2D(const Vec2& point, const Vec2& center, float radius);
const Vec2 GetNearestPointOnAABB2D(const Vec2& point, const AABB2& box);
const Vec2 GetNearestPointOnInfiniteLine2D(const Vec2& point, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine);
const Vec2 GetNearestPointOnLineSegment2D(const Vec2& point, const Vec2& start, const Vec2& end);
const Vec2 GetNearestPointOnRay2D(const Vec2& point, const Vec2& start, const Vec2& end);
const Vec2 GetNearestPointOnCapsule2D(const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius);
const Vec2 GetNearestPointOnOBB2D(const Vec2& point, const OBB2& box);
const Vec2 GetNearestPointOnPolygon2D(const Vec2& point, std::vector<Vec2> const& polygon);
const Vec2 GetNearestPointOnPolygon2DEdge(const Vec2& point, std::vector<Vec2> const& polygon);

void	GetIntersectPointsBetweenSegementsAndDisc(const Vec2& start, const Vec2& end, const Vec2& m_center, float radius, Vec2& pointA, Vec2& pointB);

bool	IsPointInsideDisk2D(const Vec2& point, const Vec2& discCenter, float discRadius);
bool	IsPointInsideAABB2D(const Vec2& point, const AABB2& box);
bool	IsPointInsideCapsule2D(const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius);
bool	IsPointInsideOBB2D(const Vec2& point, const OBB2& box);
bool    IsPointInsidePolygon2D(const Vec2& point, std::vector<Vec2> const& polygon);
bool    IsOriginInsideTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC);
void	PushDiscOutOfDisc2D(Vec2& centerMobile, float radiusMobile, const Vec2& centerFixed, float radiusFixed);
void	PushDiscsOutOfEachOther2D(Vec2& centerA, float radiusA, Vec2& centerB, float radiusB, float massRatioA = 0.5f);

void	PushDiscOutOfPoint2D(Vec2& centerMobile, float radiusMobile, const Vec2& pointFixed);
void	PushDiscOutOfAABB2D(Vec2& center, float radius, const AABB2& aabb);
void	PushDiscAndAABB2DOutOfEachOther(Vec2& center, float radius, const AABB2& aabb, Vec2& aabbCenter);

const Vec2 GetProjectedOnto2D(const Vec2& toBeProjected, const Vec2& toBeProjectedOn);
const float GetProjectedLength2D(const Vec2& toBeProjected, const Vec2& toBeProjectedOn);
const float GetAngleDegreesBetweenVectors2D(const Vec2& vecA, const Vec2& vecB);
const int GetTaxicabDistance2D(IntVec2 vecA, IntVec2 vecB);

bool IsPolygon2DConvex(std::vector<Vec2> const& polygon);
float GetApproximateRadiusOfPolygon2D(Polygon2D const& polygon);
Vec2 GetAveragePoint2D(std::vector<Vec2> const& polygon);
Vec2 GetIntersectOfTwoLine(const Vec2& startA, const Vec2& endA, const Vec2& startB, const Vec2& endB);
Disc2 GetTriangleCircumscribedCircle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC);
Disc2 GetDiscFromTwoPoints(const Vec2& pointA, const Vec2& pointB);
Disc2 GetTightBoundDiscOfPolygon(std::vector<Vec2> const& polygon);
Vec2 GetCenterOfMassOfPolygon(std::vector<Vec2> const& polygon);
Vec2 GetSupportOfPolygon(std::vector<Vec2> const& polygon, const Vec2& direction);
bool IsPointPassedOrigin(const Vec2& point, const Vec2& direction);
bool IsPolygonIntersectWithOtherPolygon(Polygon2D const& polygonA, Polygon2D const& polygonB);
Vec2 GetContactNormalOfTwoPolygons(Polygon2D const& polygonA, Polygon2D const& polygonB);
Vec2 GetPointOfTriangleNotOnClosestEdgeOfPoint(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC, const Vec2& point);
Segment2D ClipSegmentToSegment(Segment2D const& toClip, Segment2D const& refEdge);

float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStart6(float t);

float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);

float SmoothStep3(float t);

FloatRange GetRangeOnProjectedAxis(int numPoints, const Vec2* points, const Vec2& relativePos, const Vec2& axisNormal);
bool DoOBBAndOBBOverlap2D(const OBB2& boxA, const OBB2& boxB);

Vec3 SinCosPlane(float x, float z);

float GetAreaOfTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC);
float GetMomentOfTriangle(const Vec2& pointA, const Vec2& pointB, const Vec2& pointC);
IntVec2 FloorVec2ToIntVec2(Vec2 vec);

int FloatToPercent(float value);

float InfluenceFalloffExpotential(float startValue, float distance, float propagationConstant = 0.8f);

float InfluenceFalloffLinear(float startValue, float distance, float maxDistance);
float InfluenceFalloffSquare(float startValue, float distance, float maxDistance);

float GetPersonalInterestValuePower4(float oneOverRadius, int distance);
float GetPersonalInterestValuePower2(float oneOverRadius, int distance);
float GetPersonalInterestValueLinear(float oneOverRadius, int distance);


int CheckIntPositivity(int number);