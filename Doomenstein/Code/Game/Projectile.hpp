#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/IntRange.hpp"

class Projectile : public Entity
{
public:
	explicit Projectile(const EntityDefinition& def, Map* map);
public:
	//attribute
	float m_speed = 0.f;
	IntRange m_damage;
};