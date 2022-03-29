#include "Game/Projectile.hpp"
#include "Game/Map.hpp"

Projectile::Projectile(const EntityDefinition& def, Map* map)
	: Entity(def, map)
{
	m_speed = def.m_speed;
	m_damage = def.m_damage;
}