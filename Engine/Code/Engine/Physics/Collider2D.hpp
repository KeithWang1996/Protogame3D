#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Segment2D.hpp"
#include <vector>

class RenderContext;
class Physics2D;
class Rigidbody2D;
struct Plane2D;
struct Vertex_PCU;
enum eCollider2DType
{
	COLLIDER2D_DISC,
	COLLIDER2D_POLYGON,
	NUM_COLLIDER_TYPES,
};

struct PhysicsMaterial
{
	float restitution = 1.f;
	float friction = 0.f;
};

struct manifold2
{
	//Vec2 contactPoint = Vec2::ZERO;
	Segment2D contactEdge = Segment2D(Vec2::ZERO, Vec2::ZERO);
	Vec2 contactNormal = Vec2::ZERO;
	float penetration = 0;
};

// Interface for all Collider objects used with our Physics system
class Collider2D
{
	friend class Physics2D;

public: // Interface 
   // cache off the world shape representation of this object
   // taking into account the owning rigidbody (if no owner, local is world)
	virtual void UpdateWorldShape() = 0;
	//virtual Vec2 GetWorldShape() = 0;
	// queries 
	virtual Vec2 GetClosestPoint(Vec2 pos) const = 0;
	virtual bool Contains(Vec2 pos) const = 0;
	bool Intersects(Collider2D const* other) const;
	manifold2 GetManifold(Collider2D const* other) const;
	virtual bool IntersectsPlane(Plane2D const& plane) const = 0;
	virtual float GetRadius() const = 0;
	// debug helpers
	virtual void AddVerticesToDebugRender(std::vector<Vertex_PCU>& vertices, Rgba8 const& borderColor, Rgba8 const& fillColor);
	void Destroy();
	void MarkAsDestroyed();
	float GetBounciness() const { return m_physicsMaterial.restitution; }
	float GetFriction() const { return m_physicsMaterial.friction; }
	void SetBounciness(float bounciness);
	void ChangeBounciness(float offset);
	void ChangeFriction(float offset);
	float GetBounceWith(Collider2D const* other);
	float GetFrictionWith(Collider2D const* other);
	Disc2 GetWorldBounds() const { return m_bound; }

	virtual float CalculateMoment(float mass) = 0;
protected:
	Collider2D(Physics2D* system);
	virtual ~Collider2D() = 0; // private - make sure this is virtual so correct deconstructor gets called

public: // any helpers you want to add
   // ...

public:
	eCollider2DType m_type = COLLIDER2D_DISC;   // keep track of the type - will help with collision later
	Physics2D* m_system;						// system who created or destr
	Rigidbody2D* m_rigidbody = nullptr;			// owning rigidbody, used for calculating world shape
	PhysicsMaterial m_physicsMaterial;
	bool m_isDestroyed = false;
	Disc2 m_bound;
	bool m_isIntersecting = false;
	bool m_isTrigger = false;
	int m_id = 0;
private:
//Collision matrix
	typedef bool (*collision_check_cb)(Collider2D const*, Collider2D const*);
	static bool DiscVDiscCollisionCheck(Collider2D const* col0, Collider2D const* col1);
	static bool DiscVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1);
	static bool PolygonVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1);
	static collision_check_cb gCollisionChecks[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES];

	// a "matrix" lookup is just a 2D array
//Manifold matrix
	typedef manifold2(*collision_manifold_cb)(Collider2D const*, Collider2D const*);
	static manifold2 GetDiscVDiscCollisionManifold(Collider2D const* col0, Collider2D const* col1);
	static manifold2 GetDiscVPolygonCollisionManifold(Collider2D const* col0, Collider2D const* col1);
	static manifold2 GetPolygonVPolygonCollisionManifold(Collider2D const* col0, Collider2D const* col1);
	static collision_manifold_cb gManifolds[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES];
};