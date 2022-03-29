#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"

Rigidbody2D::Rigidbody2D(Physics2D* system)
	: m_system(system)
{}

Rigidbody2D::~Rigidbody2D()
{}

void Rigidbody2D::Destroy()
{
	DestroyCollider();
	m_system->DestroyRigidbody(this);
}

void Rigidbody2D::DestroyCollider()
{
	if (m_collider != nullptr)
	{
		m_system->DestroyCollider(m_collider);
		m_collider = nullptr;
	}
}

void Rigidbody2D::MarkAsDestroyed()
{
	m_isDestroyed = true;
}

void Rigidbody2D::TakeCollider(Collider2D* collider)
{
	DestroyCollider();
	m_collider = collider;
	collider->m_rigidbody = this;
	collider->UpdateWorldShape();
	CalculateMoment();
}

void Rigidbody2D::Translate(Vec2 offset)
{
	SetPosition(m_worldPosition + offset);
}

void Rigidbody2D::SetPosition(Vec2 position)
{
	m_worldPosition = position;
	if (m_collider != nullptr)
	{
		m_collider->UpdateWorldShape();
	}
}

void Rigidbody2D::ApplyImpulse(Vec2 impulse, Vec2 point)
{
	m_velocity += impulse / m_mass;
	Vec2 rT = (point - m_worldPosition).GetRotated90Degrees();
	float Torque = DotProduct2D(rT, impulse);
	m_angularVelocity += Torque / m_moment;
}

void Rigidbody2D::ApplyDragForce()
{
	m_nonGravityForces = Vec2::ZERO;//todo, move to somewhere else
	Vec2 velocity = m_velocity;//TODO, change to verlet velocity //tried but didnt work
	Vec2 dragForce = -velocity * m_drag;
	m_nonGravityForces += dragForce;
}

void Rigidbody2D::ChangeMass(float offset)
{
	m_mass += offset;
	if (m_mass < 0.001f)
	{
		m_mass = 0.001f;
	}
	CalculateMoment();
}

void Rigidbody2D::ChangeDrag(float offset)
{
	m_drag += offset;
	if (m_drag < 0.f)
	{
		m_drag = 0.f;
	}
}

Vec2 Rigidbody2D::GetVerletVelocity()
{
	return (m_worldPosition - m_startPosition) / (float)(m_system->m_fixedDeltaTime);
}

float Rigidbody2D::GetBounciness() const
{
	return m_collider->GetBounciness();
}
float Rigidbody2D::GetFriction() const
{
	return m_collider->GetFriction();
}

void Rigidbody2D::CalculateMoment()
{
	m_moment = m_collider->CalculateMoment(m_mass);
}

Vec2 Rigidbody2D::GetImpactVelocityAtPoint(Vec2 point)
{
	Vec2 Tan = (point - m_worldPosition).GetRotated90Degrees();
	Vec2 pointLocalVelocity = Tan * m_angularVelocity;
	return m_velocity + pointLocalVelocity;
}

void Rigidbody2D::SetLayer(unsigned int layer)
{
	m_layer = layer;
}

unsigned int Rigidbody2D::GetLayer()
{
	return m_layer;
}

void Rigidbody2D::SetUserData(void* user, eUserType userType)
{
	m_userData = user;
	m_userType = userType;
}

void* Rigidbody2D::GetUserData(eUserType userType)
{
	return (userType == m_userType) ? m_userData : nullptr;
}

void Rigidbody2D::SetRotation(float rotationRadians) 
{ 
	m_RotationInRadians = rotationRadians; 
	while (m_RotationInRadians < 0.f)
	{
		m_RotationInRadians += 2 * 3.14159f;
	}

	while (m_RotationInRadians > 2 * 3.14159f)
	{
		m_RotationInRadians -= 2 * 3.14159f;
	}
}

void Rigidbody2D::UpdateAngularVelocity(float offset)
{
	m_angularVelocity += offset;
}

void Rigidbody2D::SetAngularVelocity(float angularVelocity)
{
	m_angularVelocity = angularVelocity;
}