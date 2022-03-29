#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
Entity::Entity(const Vec2& position)
{
	m_position = position;
	m_velocity = Vec2(0.f, 0.f);
	m_orientationDegrees = 0.f;
	m_angularVelocity = 0.f;
	m_physicsRadius = 0.f;
	m_cosmeticRadius = 0.f;
	m_health = 3;
	m_isDead = false;
	m_isGarbage = false;
}

void Entity::Update(float deltaSeconds)
{
	m_position += deltaSeconds * m_velocity;
	m_orientationDegrees += deltaSeconds * m_angularVelocity;
}

void Entity::Die()
{
	m_isDead = true;
}

bool Entity::IsOffScreen()
{
	if ( m_position.x > m_cosmeticRadius + WORLD_SIZE_X ||
		 m_position.x < -m_cosmeticRadius ||
		 m_position.y > m_cosmeticRadius + WORLD_SIZE_Y ||
		 m_position.y < -m_cosmeticRadius )
	{
		return true;
	}
	return false;
}

Vec2 Entity::GetForwardVector()
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees, 1.f);
}

bool Entity::IsAlive() const
{
	if (m_isDead)
	{
		return false;
	}
	return true;
}

void Entity::Damaged()
{
	m_health--;
}

void Entity::RenderDebug() const
{
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer -> DrawSegment(m_position, m_position + m_velocity, 0.02f, Rgba8(255, 255, 0, 255));
	g_theRenderer -> DrawCircle(m_position, m_physicsRadius, 0.02f, Rgba8(0, 255, 255, 255));
	g_theRenderer -> DrawCircle(m_position, m_cosmeticRadius, 0.02f, Rgba8(255, 0, 255, 255));
}