#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>

class SpriteAnimSet;

class Portal : public Entity
{
public:
	explicit Portal(const EntityDefinition& def, Map* map);
	virtual void Render(const Camera& camera) const override;
	virtual void Update(float deltaSeconds) override;
public:
	std::string m_destMap = "";
	Vec2 m_destPos;
	Vec2 m_size;
	BillboradType m_billboardType;
	SpriteAnimSet* m_idleAnim = nullptr;
	float m_destYawOffset;
};