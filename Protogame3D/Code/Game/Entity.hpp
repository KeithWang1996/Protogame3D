#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
class Game;
class Entity
{
public:
	explicit Entity(const Vec2& position);
	virtual void Update(float deltaSeconds);
	virtual void Render() const = 0;
	virtual void Die();
	bool IsOffScreen();
	Vec2 GetForwardVector();
	bool IsAlive() const;
	void Damaged();

	void RenderDebug() const;

	Vec2 m_position;
	Vec2 m_velocity;
	float m_orientationDegrees;
	float m_angularVelocity;
	float m_physicsRadius;
	float m_cosmeticRadius;
	int m_health;
	bool m_isDead;
	bool m_isGarbage;
	Rgba8 m_color;
};