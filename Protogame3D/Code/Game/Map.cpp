#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"

RandomNumberGenerator Map::s_rng;
Map::Map(int sizeX, int sizeY)
{
	m_size = IntVec2(sizeX, sizeY);
	GenerateTiles();
}

int Map::GetTileIndex(const IntVec2& tileCoords) const
{
	return tileCoords.x + m_size.x * tileCoords.y;
}

const IntVec2 Map::GetTileCoordsForTileIndex(int tileIndex) const
{
	int tileY = tileIndex / m_size.x;
	int tileX = tileIndex - tileY * m_size.x;
	return IntVec2(tileX, tileY);
}

void Map::GenerateTiles()
{
	for (int tileIndex = 0; tileIndex < m_size.x * m_size.y; ++tileIndex)
	{
		IntVec2 coordinate = GetTileCoordsForTileIndex(tileIndex);
		Tile tile(coordinate);
		if (coordinate.x == 0 || coordinate.y == 0 || coordinate.x == m_size.x - 1 || coordinate.y == m_size.y - 1)
		{
			tile.m_type = TERRAIN_TYPE_STONE;
		}
		else if (!(coordinate.x >= 1 && coordinate.x <= 5 && coordinate.y >= 1 && coordinate.y <= 5)
			&& !(coordinate.x >= m_size.x - 6 && coordinate.x <= m_size.x - 2 && coordinate.y >= m_size.y - 6 && coordinate.y <= m_size.y - 2))
		{
			int random = s_rng.RollRandomIntInRange(0, 9);
			if (random < 2) {
				tile.m_type = TERRAIN_TYPE_STONE;
			}
		}
		m_tiles.push_back(tile);
	}
}

void Map::Render() const
{
	for (int tileIndex = 0; tileIndex < m_tiles.size(); ++tileIndex)
	{
		m_tiles[tileIndex].Render();
	}

	for (int entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex)
	{
		m_entities[entityIndex]->Render();
	}
}

void Map::RenderDebug() const
{
	for (int entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex)
	{
		m_entities[entityIndex]->RenderDebug();
	}
}

const AABB2 Map::GetTileBounds(const IntVec2& tileCoords) const
{
	return m_tiles[GetTileIndex(tileCoords)].GetBound();
}

void Map::AddEntity(Entity* entity)
{
	m_entities.push_back(entity);
}

void Map::Update(float deltaSeconds)
{
	for (int entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex)
	{
		m_entities[entityIndex]->Update(deltaSeconds);
		if (g_physicsOn)
		{
			int entityCoordX = RoundDownToInt(m_entities[entityIndex]->m_position.x);
			int entityCoordY = RoundDownToInt(m_entities[entityIndex]->m_position.y);
			if (m_tiles[GetTileIndex(IntVec2(entityCoordX + 1, entityCoordY))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX + 1, entityCoordY)));//right
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX - 1, entityCoordY))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX - 1, entityCoordY)));//left
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX, entityCoordY + 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX, entityCoordY + 1)));//up
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX, entityCoordY - 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX, entityCoordY - 1)));//down
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX + 1, entityCoordY + 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX + 1, entityCoordY + 1)));//up-right
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX - 1, entityCoordY + 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX - 1, entityCoordY + 1)));//up-left
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX + 1, entityCoordY - 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX + 1, entityCoordY - 1)));//down-right
			}

			if (m_tiles[GetTileIndex(IntVec2(entityCoordX - 1, entityCoordY - 1))].m_type == TERRAIN_TYPE_STONE)
			{
				PushDiscOutOfAABB2D(m_entities[entityIndex]->m_position, m_entities[entityIndex]->m_physicsRadius, GetTileBounds(IntVec2(entityCoordX - 1, entityCoordY - 1)));//down-left
			}
		}
	}
}

const Vec2 Map::GetPlayerPosition() const
{
	return m_entities[0]->m_position;
}