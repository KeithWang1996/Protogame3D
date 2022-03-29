#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Delegate.hpp"
class Collider2D;
struct Collision2D;
class Physics2D;
class RenderContext;
enum eSimulationMode
{
	DYNAMIC,
	KINEMATIC,
	STATIC,
};
enum eUserType
{
	USERTYPE_GAMEOJECT
};

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Destroy();                             // mark self for destruction, and mark collider as destruction

	void DestroyCollider();

	void MarkAsDestroyed();

	void TakeCollider(Collider2D* collider);  // takes ownership of a collider (destroying my current one if present)

	void Translate(Vec2 offset);
	void SetPosition(Vec2 position);          // update my position, and my colliders world position
	void SetVelocity(Vec2 velocity) { m_velocity = velocity; }
	void SetPhysicsEnable(bool enable) { m_physicsEnabled = enable; }
	void SetSimulationMode(eSimulationMode mode) { m_mode = mode; }
	void SetMass(float mass) { m_mass = mass; }
	float GetMass() const { return m_mass; }
	void ApplyImpulse(Vec2 impulse, Vec2 point);
	void ApplyDragForce();
	void ChangeMass(float offset);
	void ChangeDrag(float offset);

	Vec2 GetVerletVelocity();
	float GetBounciness() const;
	float GetFriction() const;

	void SetRotation(float rotationRadians);
	float GetRotation() { return m_RotationInRadians; }
	void UpdateAngularVelocity(float offset);
	void SetAngularVelocity(float angularVelocity);
	void CalculateMoment();
	Vec2 GetImpactVelocityAtPoint(Vec2 point);
	
	void SetLayer(unsigned int layer);
	unsigned int GetLayer();

	void SetUserData(void* user, eUserType userType);
	void* GetUserData(eUserType userType);
public:
	Delegate<Collision2D const&> OnOverlapBegin;
	Delegate<Collision2D const&> OnOverlapStay;
	Delegate<Collision2D const&> OnOverlapEnd;
	
	Delegate<Collision2D const&> OnTriggerBegin;
	Delegate<Collision2D const&> OnTriggerStay;
	Delegate<Collision2D const&> OnTriggerEnd;

	Physics2D* m_system;     // which scene created/owns this object
	Collider2D* m_collider = nullptr;

	Vec2 m_worldPosition = Vec2::ZERO;     // where in the world is this rigidbody
	bool m_isDestroyed = false;
	bool m_physicsEnabled = true;
	float m_mass = 1.0f;
	float m_drag = 0.f;
	Vec2 m_velocity;
	Vec2 m_nonGravityForces;
	Vec2 m_startPosition;
	//angular
	float m_RotationInRadians = 0.f;
	float m_angularVelocity = 0.f;
	float m_frameTorque = 0.f;
	float m_moment = 0.f;

	unsigned int m_layer = 0;

	eSimulationMode m_mode = DYNAMIC;
private:
	void* m_userData = nullptr;
	eUserType m_userType = USERTYPE_GAMEOJECT;
protected:
	Rigidbody2D(Physics2D* system);
	~Rigidbody2D();           // assert the collider is already null 
};