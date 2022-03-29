#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <map>
class Game;
class SpriteSheet;
class SpriteAnimSet;
class Map;
enum BillboradType
{
	CAMERA_FACING_XY,
	CAMERA_OPPOSING_XY,
	CAMERA_FACING_XYZ,
	CAMERA_OPPOSING_XYZ
};
class EntityDefinition
{
public:
	EntityDefinition(const tinyxml2::XMLElement& entityElement);
	~EntityDefinition();
public:
	static void	InitializeEntityDefinitions(const tinyxml2::XMLElement& entitiesElement);
	static const EntityDefinition* GetDefinition(const std::string& defName);
	static std::map<std::string, EntityDefinition*> s_definitions;
public:
	std::string m_name;
	std::string m_className;
	float m_radius = 0.f;
	float m_height = 0.f;
	float m_speed = 0.f;
	float m_eyeHeight = 0.f;
	Vec2  m_size;
	BillboradType m_billboardType = CAMERA_FACING_XY;
	IntRange m_damage;
	//Create/delete here
	SpriteSheet* m_spriteSheet = nullptr; 
	SpriteAnimSet* m_walkAnim = nullptr;
	SpriteAnimSet* m_attackAnim = nullptr;
	SpriteAnimSet* m_painAnim = nullptr;
	SpriteAnimSet* m_deathAnim = nullptr;
	SpriteAnimSet* m_idleAnim = nullptr;
};

class Entity
{
public:
	explicit Entity(const EntityDefinition& def, Map* map);
	virtual void Update(float deltaSeconds);
	virtual void Render(const Camera& camera) const;
	virtual void Die();
	Vec2 GetForwardVector();
	bool IsAlive() const;
	void SetPosition(const Vec2& position) { m_position = position; }
	void RenderDebug() const;
	void AddDebugToVertexArray(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices) const;
	void RotateYawDegrees(float degree);
	//attribute
	std::string m_className = "Entity";
	float m_physicsRadius = 0.f;
	float m_height = 0.f;
	//Put eyeHeight here so every entity can be possessed
	float m_eyeHeight = 0.f;
	//variable
	Vec2 m_position;
	Vec2 m_velocity;
	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 0.f;
	bool m_isDead = false;
	Map* m_map = nullptr;
	//Collision
	bool m_canBePushedByWalls = true;
	bool m_canBePushedByEntities = true;
	bool m_canPushEntities = true;
	float m_mass = 1.f;
};