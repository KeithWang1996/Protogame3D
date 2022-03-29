#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
enum TileType
{
	TERRAIN_TYPE_GRASS,
	TERRAIN_TYPE_STONE
};
class Tile
{
public:
	Tile(IntVec2 position);
	explicit Tile(int tileX, int tileY);
	void Render() const;
	AABB2 GetBound() const;


	IntVec2 m_position;
	TileType m_type = TERRAIN_TYPE_GRASS;
};