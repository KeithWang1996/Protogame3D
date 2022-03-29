#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include <vector>
class PolygonCollider2D : public Collider2D
{
	friend Physics2D;
public:
	virtual void UpdateWorldShape() override;
	// queries 
	virtual Vec2 GetClosestPoint(Vec2 pos) const override;
	Vec2 GetClosestPointEdge(Vec2 pos) const;
	virtual bool Contains(Vec2 pos) const override;
	//virtual bool Intersects(Collider2D const* other) const override;
	virtual bool IntersectsPlane(Plane2D const& plane) const override;
	// debug helpers
	virtual void AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor) override;

	virtual float GetRadius() const override { return m_radius; }

	virtual float CalculateMoment(float mass) override;

	Polygon2D GetUpdatedWorldShape() const;
public:
	Vec2 m_localPosition; // my local offset from my parent
	Vec2 m_worldPosition; // my local offset from my parent
	float m_localRotation = 0.f;
	float m_worldRotation = 0.f;
	Polygon2D* m_polygon;
	float m_radius;//very approximate radius for out screen calculation
protected:
	PolygonCollider2D(Physics2D* system, Vec2 const& localPosition, std::vector<Vec2> const vertices, bool isGiftWrapping = false);
	~PolygonCollider2D();
};