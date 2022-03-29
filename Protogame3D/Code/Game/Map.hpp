#pragma once
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
class Entity;
class Map
{
public:
	Map(int sizeX, int sizeY);
	void Render() const;
	void RenderDebug() const;
	int GetTileIndex(const IntVec2& tileCoords) const;
	const IntVec2 GetTileCoordsForTileIndex(int tileIndex) const;
	const AABB2 GetTileBounds(const IntVec2& tileCoords) const;
	void GenerateTiles();
	void Update(float deltaSeconds);
	void AddEntity(Entity* entity);
	const Vec2 GetPlayerPosition() const;
private:
	std::vector< Tile > m_tiles;
	std::vector< Entity* > m_entities;
	IntVec2 m_size;
	static RandomNumberGenerator s_rng;
};