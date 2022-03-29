#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/Vec3.hpp"

Collision2D Collision2D::GetInversed()
{
	Collision2D invCollision;
	invCollision.manifold.contactNormal = -manifold.contactNormal;
	invCollision.me = them;
	invCollision.them = me;
	invCollision.m_colliderID = m_colliderID;
	invCollision.m_frameId = m_frameId;
	return invCollision;
}

Physics2D::Physics2D()
{
	m_ground = new Plane2D(Vec2(0.f, 1.f), 0.f);
	for (int i = 0; i < 32; ++i)
	{
		m_layerInteractions[i] = 0xffffffff;
	}
}
Physics2D::~Physics2D()
{
	delete m_ground;
	m_ground = nullptr;
	for (int rigidbodyIndex = 0; rigidbodyIndex < m_rigidbodies.size(); rigidbodyIndex++)
	{
		delete m_rigidbodies[rigidbodyIndex];
	}

	for (int colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++)
	{
		delete m_colliders[colliderIndex];
	}
	m_rigidbodies.clear();
	m_colliders.clear();
}

void Physics2D::BeginFrame()
{}

void Physics2D::Update()
{
	while (m_stepTimer.CheckAndDecrement()) {
		AdvanceSimulation((float)m_fixedDeltaTime);
		++m_frameId;
	}
}

void Physics2D::AdvanceSimulation(float deltaSeconds)
{
	ApplyEffectors(deltaSeconds);
	MoveRigidbodies(deltaSeconds);
	DetectCollisions();
	ResolveCollisions();
	CleanupDestroyedObjects();
}

void Physics2D::ApplyEffectors(float deltaSeconds)
{
	Vec2 frameGravityAcceleration = m_gravity * deltaSeconds;
	for (int rigidbodyIndex = 0; rigidbodyIndex < m_rigidbodies.size(); ++rigidbodyIndex)
	{
		Rigidbody2D* rigidbody = m_rigidbodies[rigidbodyIndex];
		//update start position here
		rigidbody->m_startPosition = rigidbody->m_worldPosition;
		if (rigidbody->m_physicsEnabled)
		{
			if (rigidbody->m_mode == DYNAMIC)
			{
				rigidbody->ApplyDragForce();
				Vec2 frameOtherAcceleration = (rigidbody->m_nonGravityForces / rigidbody->m_mass) * deltaSeconds;
				rigidbody->SetVelocity(rigidbody->m_velocity + frameGravityAcceleration + frameOtherAcceleration);
			}
			else if (rigidbody->m_mode == STATIC)
			{
				rigidbody->SetVelocity(Vec2::ZERO);
			}
		}
		else
		{
			rigidbody->SetVelocity(Vec2::ZERO);
		}
	}
}

void Physics2D::MoveRigidbodies(float deltaSeconds)
{
	for (int rigidbodyIndex = 0; rigidbodyIndex < m_rigidbodies.size(); ++rigidbodyIndex)
	{
		Rigidbody2D* rigidbody = m_rigidbodies[rigidbodyIndex];
		if (rigidbody->m_mode == STATIC || !rigidbody->m_physicsEnabled)
		{
			continue;
		}
		rigidbody->SetPosition(rigidbody->m_worldPosition + rigidbody->m_velocity * deltaSeconds);
		rigidbody->SetRotation(rigidbody->m_RotationInRadians + rigidbody->m_angularVelocity * deltaSeconds);
	}
}

void Physics2D::CleanupDestroyedObjects()
{
	for (int rigidbodyIndex = 0; rigidbodyIndex < m_rigidbodies.size(); rigidbodyIndex++)
	{
		if (m_rigidbodies[rigidbodyIndex]->m_isDestroyed)
		{
			delete m_rigidbodies[rigidbodyIndex];
			m_rigidbodies.erase(m_rigidbodies.begin() + rigidbodyIndex);
			rigidbodyIndex--;
		}
	}

	for (int colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++)
	{
		if (m_colliders[colliderIndex]->m_isDestroyed)
		{
			delete m_colliders[colliderIndex];
			m_colliders.erase(m_colliders.begin() + colliderIndex);
			colliderIndex--;
		}
	}
}

void Physics2D::EndFrame()
{}

void Physics2D::SetGroundHeight(float height)
{
	m_ground->distanceFromOriginAlongNormal = height;
}

Rigidbody2D* Physics2D::CreateRigidbody()
{
	Rigidbody2D* rigidbody = new Rigidbody2D(this);
	m_rigidbodies.push_back(rigidbody);
	return rigidbody;
}

void Physics2D::DestroyRigidbody(Rigidbody2D* rb)
{
	rb->MarkAsDestroyed();
}

DiscCollider2D* Physics2D::CreateDiscCollider(Vec2 localPosition, float radius)
{
	DiscCollider2D* collider = new DiscCollider2D(this, localPosition, radius);
	collider->m_id = m_colliderId;
	++m_colliderId;
	m_colliders.push_back(collider);
	return collider;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider(Vec2 localPosition, std::vector<Vec2> const& points, bool isGiftWrapping)
{
	PolygonCollider2D* collider = new PolygonCollider2D(this, localPosition, points, isGiftWrapping);
	collider->m_id = m_colliderId;
	++m_colliderId;
	m_colliders.push_back(collider);
	return collider;
}

void Physics2D::DestroyCollider(Collider2D* collider)
{
	collider->MarkAsDestroyed();
}

void Physics2D::DetectAllCollisionsFor(Collider2D* collider)
{
	if (collider->m_rigidbody->m_mode == STATIC)
	{
		return;
	}
	for (int colliderIndex = 0; colliderIndex < m_colliders.size(); ++colliderIndex)
	{
		if (collider != m_colliders[colliderIndex] && collider->Intersects(m_colliders[colliderIndex]))
		{
			Collision2D collision;
			manifold2 manifold = collider->GetManifold(m_colliders[colliderIndex]);
			collision.me = collider;
			collision.them = m_colliders[colliderIndex];
			m_unresolvedCollisions.push(collision);
		}
	}
}

void Physics2D::SetFixedDeltaTime(double frameTimeSeconds)
{
	m_fixedDeltaTime = frameTimeSeconds;
	m_stepTimer.SetSeconds(m_fixedDeltaTime);
}

void Physics2D::SetClock(Clock* clock)
{
	m_clock = clock;
	m_stepTimer.SetSeconds(m_clock, m_fixedDeltaTime);
}

void Physics2D::EnableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1)
{
	m_layerInteractions[layerIdx0] |= 1 << layerIdx1;
	m_layerInteractions[layerIdx1] |= 1 << layerIdx0;
}

void Physics2D::DisableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1)
{
	m_layerInteractions[layerIdx0] &= ~(1 << layerIdx1);
	m_layerInteractions[layerIdx1] &= ~(1 << layerIdx0);
}

bool Physics2D::DoLayersInteract(unsigned int layerIdx0, unsigned int layerIdx1) const
{
	unsigned int t1 = m_layerInteractions[layerIdx0];
	unsigned int t2 = 1 << layerIdx1;
	unsigned int t3 = t1 & t2;
	return (t3 != 0);
}

void Physics2D::DetectCollisions()
{
	if (m_colliders.size() < 2)
	{
		return;
	}
	for (int colliderIndex = 0; colliderIndex < m_colliders.size(); ++colliderIndex)
	{
		for (int colliderIndexJ = colliderIndex + 1; colliderIndexJ < m_colliders.size(); ++colliderIndexJ)
		{
			Collider2D* me = m_colliders[colliderIndex];
			Collider2D* them = m_colliders[colliderIndexJ];
			if ((me->m_rigidbody->m_mode != STATIC || them->m_rigidbody->m_mode != STATIC) &&
				me->Intersects(them))
			{
				me->m_isIntersecting = true;
				them->m_isIntersecting = true;
// 				if (me->m_rigidbody->m_collider->m_type == COLLIDER2D_POLYGON &&
// 					them->m_rigidbody->m_collider->m_type == COLLIDER2D_POLYGON)
// 				{
// 					manifold2 manifold = me->GetManifold(them);
// 					return;
// 				}
				Collision2D collision;
				collision.m_colliderID = IntVec2(std::min(me->m_id, them->m_id), std::max(me->m_id, them->m_id));
				collision.m_frameId = m_frameId;
				manifold2 manifold = me->GetManifold(them);
// 				DebugAddWorldPoint(Vec3(manifold.contactEdge.pointA, 0.f) + Vec3(20.f, 10.f, 0.f), 1.f, Rgba8(255, 0, 0, 255), 2.f);
// 				DebugAddWorldPoint(Vec3(manifold.contactEdge.pointB, 0.f) + Vec3(20.f, 10.f, 0.f), 1.f, Rgba8(255, 0, 0, 255), 2.f);
				collision.me = me;
				collision.them = them;
				collision.manifold = manifold;
				m_unresolvedCollisions.push(collision);

				bool wasInLastFrame = false;
				for (int i = 0; i < m_collisionsForEvents.size(); ++i)
				{
					if (m_collisionsForEvents[i].m_colliderID == collision.m_colliderID)
					{
						m_collisionsForEvents[i].m_frameId = m_frameId;
						if ((me->m_isTrigger || them->m_isTrigger))
						{
							if (me->m_rigidbody->m_layer == them->m_rigidbody->m_layer)
							{
								me->m_rigidbody->OnTriggerStay(collision);
								them->m_rigidbody->OnTriggerStay(collision.GetInversed());
							}
						}
						else if(DoLayersInteract(me->m_rigidbody->m_layer, them->m_rigidbody->m_layer))
						{
							me->m_rigidbody->OnOverlapStay(collision);
							them->m_rigidbody->OnOverlapStay(collision.GetInversed());
						}
						wasInLastFrame = true;
						break;
					}
				}
				if (!wasInLastFrame)
				{
					m_collisionsForEvents.push_back(collision);
					if ((me->m_isTrigger || them->m_isTrigger))
					{
						if (me->m_rigidbody->m_layer == them->m_rigidbody->m_layer)
						{
							me->m_rigidbody->OnTriggerBegin(collision);
							them->m_rigidbody->OnTriggerBegin(collision.GetInversed());
						}
						
					}
					else if(DoLayersInteract(me->m_rigidbody->m_layer, them->m_rigidbody->m_layer))
					{
						me->m_rigidbody->OnOverlapBegin(collision);
						them->m_rigidbody->OnOverlapBegin(collision.GetInversed());
					}
				}

			}
			else
			{
				me->m_isIntersecting = false;
				them->m_isIntersecting = false;
			}
		}
	}
	for (int i = 0; i < m_collisionsForEvents.size(); ++i)
	{
		if (m_collisionsForEvents[i].m_frameId < m_frameId)
		{
			if ((m_collisionsForEvents[i].me->m_isTrigger || m_collisionsForEvents[i].them->m_isTrigger))
			{
				if (m_collisionsForEvents[i].me->m_rigidbody->m_layer == m_collisionsForEvents[i].them->m_rigidbody->m_layer)
				{
					m_collisionsForEvents[i].me->m_rigidbody->OnTriggerEnd(m_collisionsForEvents[i]);
					m_collisionsForEvents[i].them->m_rigidbody->OnTriggerEnd(m_collisionsForEvents[i].GetInversed());
				}
			}
			else if(DoLayersInteract(m_collisionsForEvents[i].me->m_rigidbody->m_layer, m_collisionsForEvents[i].them->m_rigidbody->m_layer))
			{
				m_collisionsForEvents[i].me->m_rigidbody->OnOverlapEnd(m_collisionsForEvents[i]);
				m_collisionsForEvents[i].them->m_rigidbody->OnOverlapEnd(m_collisionsForEvents[i].GetInversed());
			}

			m_collisionsForEvents.erase(m_collisionsForEvents.begin() + i);
		}
	}
}

void Physics2D::ResolveCollisions()
{
	while (!m_unresolvedCollisions.empty())
	{
		ResolveCollision(m_unresolvedCollisions.front());
		m_unresolvedCollisions.pop();
	}
}

void Physics2D::ResolveCollision(Collision2D const& collision)
{
	if (collision.me->m_isTrigger || collision.them->m_isTrigger || 
		!DoLayersInteract(collision.me->m_rigidbody->m_layer, collision.them->m_rigidbody->m_layer))
	{
		return;
	}
	CorrectObjectsInCollision(collision);
	ApplyImpulses(collision);
}

void Physics2D::CorrectObjectsInCollision(Collision2D const& collision)
{
	// Correcting - stop overlapping
	eSimulationMode meMode = collision.me->m_rigidbody->m_mode;
	eSimulationMode theirMode = collision.them->m_rigidbody->m_mode;
	float myMass = collision.me->m_rigidbody->GetMass();
	float theirMass = collision.them->m_rigidbody->GetMass();
	if (meMode > theirMode)
	{
		collision.them->m_rigidbody->Translate(-collision.GetNormal() * collision.GetPenetration());
	}
	else if (meMode < theirMode)
	{
		collision.me->m_rigidbody->Translate(collision.GetNormal() * collision.GetPenetration());
	}
	else
	{
		float pushMe = theirMass / (myMass + theirMass);
		float pushThem = 1.0f - pushMe;
		collision.me->m_rigidbody->Translate(pushMe * collision.GetNormal() * collision.GetPenetration());
		collision.them->m_rigidbody->Translate(-pushThem * collision.GetNormal() * collision.GetPenetration());
	}
	
	//Apply Impulse

	
}

float Physics2D::CalculateNormalImpulses(Collision2D const& collision)
{
	float myMass = collision.me->m_rigidbody->GetMass();
	float theirMass = collision.them->m_rigidbody->GetMass();
	if (collision.me->m_rigidbody->m_mode != DYNAMIC)
	{
		myMass = INFINITY;
	}
	else if (collision.them->m_rigidbody->m_mode != DYNAMIC)
	{
		theirMass = INFINITY;
	}
	Vec2 contactPoint = (collision.manifold.contactEdge.pointA + collision.manifold.contactEdge.pointB) * 0.5f;
	Vec2 myVelocity = collision.me->m_rigidbody->GetImpactVelocityAtPoint(contactPoint);
	Vec2 theirVelocity = collision.them->m_rigidbody->GetImpactVelocityAtPoint(contactPoint);

	Vec2 diffVelocity = myVelocity - theirVelocity;
	Vec2 normal = collision.GetNormal();
	Vec2 rAPT = (contactPoint - collision.me->m_rigidbody->m_worldPosition).GetRotated90Degrees();
	float rAPTDotNormal = DotProduct2D(rAPT, normal);
	Vec2 rBPT = (contactPoint - collision.them->m_rigidbody->m_worldPosition).GetRotated90Degrees();
	float rBPTDotNormal = DotProduct2D(rBPT, normal);
	float bounciness = collision.me->GetBounceWith(collision.them);
	float impulse = -DotProduct2D((1 + bounciness) * diffVelocity, normal) /
		(DotProduct2D(normal, normal) * (1.f / myMass + 1.f / theirMass) +
			rAPTDotNormal * rAPTDotNormal / collision.me->m_rigidbody->m_moment +
			rBPTDotNormal * rBPTDotNormal / collision.them->m_rigidbody->m_moment);
	return impulse;
}

float Physics2D::CalculateTangentImpulses(Collision2D const& collision)
{
	float myMass = collision.me->m_rigidbody->GetMass();
	float theirMass = collision.them->m_rigidbody->GetMass();
	if (collision.me->m_rigidbody->m_mode != DYNAMIC)
	{
		myMass = INFINITY;
	}
	else if (collision.them->m_rigidbody->m_mode != DYNAMIC)
	{
		theirMass = INFINITY;
	}
	Vec2 contactPoint = (collision.manifold.contactEdge.pointA + collision.manifold.contactEdge.pointB) * 0.5f;
	Vec2 myVelocity = collision.me->m_rigidbody->GetImpactVelocityAtPoint(contactPoint);
	Vec2 theirVelocity = collision.them->m_rigidbody->GetImpactVelocityAtPoint(contactPoint);

	Vec2 diffVelocity = myVelocity - theirVelocity;
	Vec2 tangent = collision.GetNormal().GetRotated90Degrees();
	Vec2 rAPT = (contactPoint - collision.me->m_rigidbody->m_worldPosition).GetRotated90Degrees();
	float rAPTDotNormal = DotProduct2D(rAPT, tangent);
	Vec2 rBPT = (contactPoint - collision.them->m_rigidbody->m_worldPosition).GetRotated90Degrees();
	float rBPTDotNormal = DotProduct2D(rBPT, tangent);
	float bounciness = collision.me->GetBounceWith(collision.them);
	float impulse = -DotProduct2D((1 + bounciness) * diffVelocity, tangent) /
		(DotProduct2D(tangent, tangent) * (1.f / myMass + 1.f / theirMass) +
			rAPTDotNormal * rAPTDotNormal / collision.me->m_rigidbody->m_moment +
			rBPTDotNormal * rBPTDotNormal / collision.them->m_rigidbody->m_moment);
	return impulse;
}

void Physics2D::ApplyImpulses(Collision2D const& collision)
{
	eSimulationMode meMode = collision.me->m_rigidbody->m_mode;
	eSimulationMode theirMode = collision.them->m_rigidbody->m_mode;
	if (meMode != DYNAMIC && theirMode != DYNAMIC)
	{
		return;
	}
	float normalImpulse = CalculateNormalImpulses(collision);
	float friction = collision.me->GetFrictionWith(collision.them);
	float tanImpulse = CalculateTangentImpulses(collision);
	Vec2 contactPoint = (collision.manifold.contactEdge.pointA + collision.manifold.contactEdge.pointB) * 0.5f;
	
	Vec2 tangent = collision.GetNormal().GetRotated90Degrees();
	if (abs(tanImpulse) > friction * normalImpulse) {
		tanImpulse = SignFloat(tanImpulse) * normalImpulse * friction;
	}
	Vec2 jn = normalImpulse * collision.GetNormal() + tanImpulse * tangent;
	Vec2 meApplyPoint = GetNearestPointOnLineSegment2D(collision.me->m_rigidbody->m_worldPosition,
		collision.manifold.contactEdge.pointA, collision.manifold.contactEdge.pointB);
	Vec2 themApplyPoint = GetNearestPointOnLineSegment2D(collision.them->m_rigidbody->m_worldPosition,
		collision.manifold.contactEdge.pointA, collision.manifold.contactEdge.pointB);
	if (meMode == DYNAMIC && theirMode == DYNAMIC)
	{
		collision.me->m_rigidbody->ApplyImpulse(jn, meApplyPoint);
		collision.them->m_rigidbody->ApplyImpulse(-jn, themApplyPoint);

	}
	else if (meMode == DYNAMIC)
	{
		collision.me->m_rigidbody->ApplyImpulse(jn, meApplyPoint);
	}
	else if (theirMode == DYNAMIC)
	{
		collision.them->m_rigidbody->ApplyImpulse(-jn, themApplyPoint);
	}
}

Vec2 Collision2D::GetNormal() const
{
	return manifold.contactNormal;
}

float Collision2D::GetPenetration() const
{
	return manifold.penetration;
}