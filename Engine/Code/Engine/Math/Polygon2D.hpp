#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>
class Polygon2D
{
public:
	bool IsValid() const; // must have at least 3 points to be considered a polygon

	bool IsConvex() const;
	bool Contains(Vec2 point) const;
	float GetDistance(Vec2 point) const;
	Vec2 GetClosestPoint(Vec2 point) const;
	Vec2 GetClosestPointOnEdge(Vec2 point) const;
	// accessors
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge(int idx, Vec2& outStart, Vec2& outEnd);
	Vec2 GetCenter() const { return m_center; }
	void SetCenter(Vec2 center) { m_center = center; }
	std::vector<Vec2> GetPoints() const { return m_points; }
	void Translate(Vec2 diff);
	void TranslateTo(Vec2 dest);
	void Rotate(float rotationDegrees);
	float GetMomentumWithoutMass();
	Vec2 GetSupport(const Vec2& direction) const;
public: // static constructors (feel free to just use a constructor - I just like descriptive names)
		// in this case, these two take the same parameters but behave differently

	// construct from a counter-clockwise line loop
	//static Polygon2D MakeFromLineLoop(Vec2 const* points, unsigned int pointCount);

	// create a convex wrapping of a collection of points;  
	//static Polygon2D MakeConvexFromPointCloud(Vec2 const* points, unsigned int pointCount);

	Polygon2D(Vec2 const* points, unsigned int pointCount);
	Polygon2D(std::vector<Vec2> const& points);
private:
	std::vector<Vec2> m_points;
	Vec2 m_center;
};