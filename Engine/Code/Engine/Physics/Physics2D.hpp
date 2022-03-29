#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Core/Timer.hpp"
#include <vector>
#include <queue>
class Rigidbody2D;
class Collider2D;
class DiscCollider2D;
class PolygonCollider2D;
struct Plane2D;
class Clock;
struct Collision2D
{
	manifold2 manifold;
	Collider2D* me = nullptr;
	Collider2D* them = nullptr;
	Vec2 GetNormal() const;
	float GetPenetration() const;
	IntVec2 m_colliderID;
	int m_frameId = 0;
public:
	Collision2D GetInversed();
};

class Physics2D
{
public:
	Physics2D();
	~Physics2D();
	void BeginFrame();

	void Update();      // nothing in A01, but eventually it is the update, collision detection, and collision response part
	void ApplyEffectors(float deltaSeconds);
	void MoveRigidbodies(float deltaSeconds);
	void CleanupDestroyedObjects();
	void EndFrame();    // cleanup destroyed objects
	void SetGroundHeight(float height);
	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyRigidbody(Rigidbody2D* rb);

	DiscCollider2D* CreateDiscCollider(Vec2 localPosition, float radius);
	PolygonCollider2D* CreatePolygonCollider(Vec2 localPosition, std::vector<Vec2> const& points, bool isGiftWrapping = false);
	void DestroyCollider(Collider2D* collider);

	void DetectAllCollisionsFor(Collider2D* collider);
	void SetSceneGravity(Vec2 gravity) { m_gravity = gravity; }
	double GetFixedDeltaTime() const { return m_fixedDeltaTime; }
	void SetFixedDeltaTime(double frameTimeSeconds);
	void SetClock(Clock* clock);

	void EnableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1);
	void DisableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1);
	bool DoLayersInteract(unsigned int layerIdx0, unsigned int layerIdx1) const;
public:
	int m_colliderId = 0;
	int m_frameId = 0;
	std::vector<Rigidbody2D*> m_rigidbodies;
	std::vector<Collider2D*> m_colliders;
	Vec2 m_gravity = Vec2::ZERO;//in form of acceleration
	Plane2D* m_ground = nullptr;
	double m_fixedDeltaTime = 1.0 / 120.f;
	// add members you may need to store these
	// storage for all rigidbodies
	// storage for all colliders
	// ...
	std::queue<Collision2D> m_unresolvedCollisions;
	std::vector<Collision2D> m_collisionsForEvents;
	Clock* m_clock = nullptr;
	Timer m_stepTimer;

	unsigned int m_layerInteractions[32];
private:
	void AdvanceSimulation(float deltaSeconds);
	void DetectCollisions();
	void ResolveCollisions();
	void ResolveCollision(Collision2D const& collision);
	void CorrectObjectsInCollision(Collision2D const& collision);
	void ApplyImpulses(Collision2D const& collision);
	//helper
	float CalculateNormalImpulses(Collision2D const& collision);
	float CalculateTangentImpulses(Collision2D const& collision);
};