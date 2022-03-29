#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vec3.hpp"
class Entity;
class Actor;
class Projectile;
class Portal;
struct RaycastResult
{
	Vec3 startPosition;
	Vec3 forwardNormal;
	float maxDistance = 0.f;
	bool didImpact = false;
	Vec3 impactPosition;
	Entity* impactEntity = nullptr;
	float impactFraction = 0.f;
	float impactDistance = 0.f;
	Vec3 impactSurfaceNormal;
};
class Map
{
public:
	Map() = default;
	~Map();
	virtual void Render(const Camera& camera) const = 0;
	void RenderDebug() const;
	virtual void UpdateMeshes() = 0;
	virtual Entity* SpawnNewEntityOfType(const std::string& entityDefName);
	virtual Entity* SpawnNewEntityOfType(const EntityDefinition& entityDef);
	void AddEntityToMap(const std::string& entityDefName, Vec2 pos, float yaw);
	Entity* GetEntityCanBePossessed(const Camera& camera);
	virtual RaycastResult Raycast(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const = 0;
	//virtual void RenderDebug() const = 0;
	//int GetTileIndex(const IntVec2& tileCoords) const;
	//const IntVec2 GetTileCoordsForTileIndex(int tileIndex) const;
	//const AABB2 GetTileBounds(const IntVec2& tileCoords) const;
	//void GenerateTiles();
	virtual void Update(float deltaSeconds);
	//void AddEntity(Entity* entity);
	//const Vec2 GetPlayerPosition() const;
public:
	Vec2 m_playerStart;
	float m_playerYaw = 0;
	std::string m_name;
	std::vector< Entity* > m_allEntities;
	std::vector< Entity* > m_actors;
	std::vector< Entity* > m_projectiles;
};