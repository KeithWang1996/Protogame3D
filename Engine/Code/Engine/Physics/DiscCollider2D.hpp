#pragma once
#include "Engine/Physics/Collider2D.hpp"
class DiscCollider2D : public Collider2D
{
	friend class Physics2D;
public: // implement the interface of Collider2D
   // ...
	virtual void UpdateWorldShape() override;
	//virtual Vec2 GetWorldShape() override;
	virtual Vec2 GetClosestPoint(Vec2 pos) const override;
	virtual bool Contains(Vec2 pos) const override;
	//virtual bool Intersects(Collider2D const* other) const override;
	virtual void AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor) override;
	virtual bool IntersectsPlane(Plane2D const& plane) const override;
	virtual float GetRadius() const override { return m_radius; };
	virtual float CalculateMoment(float mass) override;
public:
	Vec2 m_localPosition; // my local offset from my parent
	Vec2 m_worldPosition; // calculated from local position and owning rigidbody if present
	float m_localRotation = 0.f;
	float m_worldRotation = 0.f;
	float m_radius;
protected:
	DiscCollider2D(Physics2D* system, Vec2 const& localPosition, float radius);
	~DiscCollider2D() = default;
};