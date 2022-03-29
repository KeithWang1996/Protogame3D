#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
class SpriteSheet;
class SpriteAnimSet;

class Actor : public Entity
{
public:
	explicit Actor(const EntityDefinition& def, Map* map);
	virtual void Render(const Camera& camera) const override;
public:
	//attribute
	float m_walkSpeed;
	Vec2 m_size;
	BillboradType m_billboardType;
	SpriteAnimSet* m_walkAnim = nullptr;
	SpriteAnimSet* m_attackAnim = nullptr;
	SpriteAnimSet* m_painAnim = nullptr;
	SpriteAnimSet* m_deathAnim = nullptr;
};