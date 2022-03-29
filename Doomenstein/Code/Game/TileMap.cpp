#include "Game/TileMap.hpp"
#include "Game/Entity.hpp"
#include "Game/Portal.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/DebugRender.hpp"
TileMap::TileMap(int sizeX, int sizeY)
{
	m_dimensions = IntVec2(sizeX, sizeY);
	m_mesh = new GPUMesh(g_theRenderer);
	//GenerateTiles();
	UpdateMeshes();
}

TileMap::TileMap(const tinyxml2::XMLElement& mapElement)
{
	m_dimensions = ParseXmlAttribute(mapElement, "dimensions", IntVec2::ZERO);
	std::map<char, std::string> legends;
	const tinyxml2::XMLElement* legendElement = mapElement.FirstChildElement("Legend");
	const tinyxml2::XMLElement* tileElement = legendElement->FirstChildElement();
	//loading legends
	while (tileElement)
	{
		char glyph = ParseXmlAttribute(*tileElement, "glyph", ' ');
		std::string regionType = ParseXmlAttribute(*tileElement, "regionType", "");
		legends[glyph] = regionType;
		tileElement = tileElement->NextSiblingElement();
	}
	const tinyxml2::XMLElement* mapRowsElement = legendElement->NextSiblingElement();
	const tinyxml2::XMLElement* mapRowElement = mapRowsElement->FirstChildElement();
	
	int tileY = m_dimensions.y - 1;
	while (mapRowElement)
	{
		std::string rowString = ParseXmlAttribute(*mapRowElement, "tiles", "");
		for (int tileX = 0; tileX < rowString.size(); ++tileX)
		{
			if (rowString.size() != m_dimensions.x)
			{
				g_theConsole->Error("Map width does not match.");
			}
			std::string regionTypeName = legends[rowString[tileX]];
			MapRegionType* regionType = MapRegionType::s_definitions[regionTypeName];
			if (!regionType)
			{
				g_theConsole->Error("Region " + regionTypeName + " not found.");
				regionType = MapRegionType::errorRegion;
			}
			Tile tile = Tile(IntVec2(tileX, tileY), regionType);
			m_tiles.push_back(tile);
		}
		--tileY;
		mapRowElement = mapRowElement->NextSiblingElement();
	}
	if (tileY != -1)
	{
		g_theConsole->Error("Map height does not match.");
	}

	const tinyxml2::XMLElement* entitiesElement = mapRowsElement->NextSiblingElement();
	const tinyxml2::XMLElement* playerStartElement = entitiesElement->FirstChildElement();

	m_playerStart = ParseXmlAttribute(*playerStartElement, "pos", Vec2::ZERO);
	m_playerYaw = ParseXmlAttribute(*playerStartElement, "yaw", 0.f);
	const tinyxml2::XMLElement* entityElement = playerStartElement->NextSiblingElement();
	while (entityElement)
	{
		std::string type = ParseXmlAttribute(*entityElement, "type", "Marine");
		std::string className = entityElement->Name();
		Entity* newEntity = SpawnNewEntityOfType(type);
		if (newEntity)
		{
			newEntity->m_position = ParseXmlAttribute(*entityElement, "pos", Vec2::ZERO);
			newEntity->m_orientationDegrees = ParseXmlAttribute(*entityElement, "yaw", 0.f);
			if (className == "Portal")
			{
				Portal* newPortal = (Portal*)newEntity;
				newPortal->m_destMap = ParseXmlAttribute(*entityElement, "destMap", "");
				newPortal->m_destPos = ParseXmlAttribute(*entityElement, "destPos", Vec2::ZERO);
				newPortal->m_destYawOffset = ParseXmlAttribute(*entityElement, "destYawOffset", 0.f);
			}
		}
		entityElement = entityElement->NextSiblingElement();
	}

	//Create Mesh
	m_mesh = new GPUMesh(g_theRenderer);
	UpdateMeshes();
}

TileMap::~TileMap()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void TileMap::Render(const Camera& camera) const
{
// 	for (int i = 0; i < m_tiles.size(); ++i)
// 	{
// 		m_tiles[i].Render(); //outdated
// 	}
	for (int i = 0; i < m_allEntities.size(); ++i)
	{
		m_allEntities[i]->Render(camera);
	}
	g_theRenderer->BindTexture(&MapMaterialType::s_spriteSheet["TestTerrain"]->GetTexture());
	g_theRenderer->DrawMesh(m_mesh);

	if (g_debugDraw)
	{
		RaycastResult result = Raycast(camera.GetPosition(), camera.GetForward(), 5.f);
		//DebugAddWorldLine(result.startPosition, result.impactPosition, Rgba8::WHITE, 0.1f);

		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->SetDepthTest(COMPARE_FUNC_ALWAYS, 1.f);
		//g_theRenderer->DrawSegment3D(result.startPosition, result.impactPosition, 0.03f, Rgba8::WHITE);
		if (result.impactSurfaceNormal.z > 0.5f)//TODO fix problem cylinder not show when z = -1
		{
			g_theRenderer->DrawSegment3D(result.impactPosition, result.impactPosition + result.impactSurfaceNormal * 0.2f, 0.01f, Rgba8::WHITE);
		}
		else
		{
			g_theRenderer->DrawSegment3D(result.impactPosition + result.impactSurfaceNormal * 0.2f, result.impactPosition, 0.01f, Rgba8::WHITE);

		}
	}
}

void TileMap::Update(float deltaSeconds)
{
	Map::Update(deltaSeconds);
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); ++entityIndex)
	{
		HandleEntitiesAgainstWall(m_allEntities[entityIndex]);
	}
}

void TileMap::UpdateMeshes()
{
	std::vector<unsigned int> indices;
	std::vector<Vertex_PCU> vertices;
	// Add Quads for map
	for (int i = 0; i < m_tiles.size(); ++i)
	{
		IntVec2 tileCoord = m_tiles[i].m_position;
		Vec2 coord = Vec2((float)tileCoord.x, (float)tileCoord.y);
		if (m_tiles[i].m_regionType->m_isSolid)
		{
			//four sides
			MapMaterialType* sideMaterial = m_tiles[i].m_regionType->m_sideMaterial;
			Vec2 uvMins, uvMaxs;
			sideMaterial->m_sheet->GetSpriteUVs(uvMins, uvMaxs, sideMaterial->m_spriteCoords);
			//South y
			if ((i + m_dimensions.x) < m_tiles.size() && !m_tiles[i + m_dimensions.x].m_regionType->m_isSolid)
			{
				AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord, 0), Vec3(coord.x + 1, coord.y, 0), Vec3(coord.x + 1, coord.y, 1), Vec3(coord, 1), Rgba8::WHITE, uvMins, uvMaxs);
			}

			//East x+1
			if (m_tiles[i].m_position.x < m_dimensions.x - 1 && !m_tiles[i + 1].m_regionType->m_isSolid)
			{
				AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord.x + 1, coord.y, 0), Vec3(coord.x + 1, coord.y + 1, 0), Vec3(coord.x + 1, coord.y + 1, 1), Vec3(coord.x + 1, coord.y, 1), Rgba8::WHITE, uvMins, uvMaxs);
			}
			
			//North y+1
			if ((i - m_dimensions.x) >= 0 && !m_tiles[i - m_dimensions.x].m_regionType->m_isSolid)
			{
				AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord.x + 1, coord.y + 1, 0), Vec3(coord.x, coord.y + 1, 0), Vec3(coord.x, coord.y + 1, 1), Vec3(coord.x + 1, coord.y + 1, 1), Rgba8::WHITE, uvMins, uvMaxs);
			}
			
			
			//West x
			if (m_tiles[i].m_position.x > 1 && !m_tiles[i - 1].m_regionType->m_isSolid)
			{
				AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord.x, coord.y + 1, 0), Vec3(coord.x, coord.y, 0), Vec3(coord.x, coord.y, 1), Vec3(coord.x, coord.y + 1, 1), Rgba8::WHITE, uvMins, uvMaxs);
			}			
		}
		else
		{
			//floor
			MapMaterialType* floorMaterial = m_tiles[i].m_regionType->m_floorMaterial;
			Vec2 uvMins, uvMaxs;
			floorMaterial->m_sheet->GetSpriteUVs(uvMins, uvMaxs, floorMaterial->m_spriteCoords);
			AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord, 0), Vec3(coord.x + 1, coord.y, 0), Vec3(coord.x + 1, coord.y + 1, 0), Vec3(coord.x, coord.y + 1, 0), Rgba8::WHITE, uvMins, uvMaxs);
			
			//ceiling
			MapMaterialType* ceilingMaterial = m_tiles[i].m_regionType->m_ceilingMaterial;
			ceilingMaterial->m_sheet->GetSpriteUVs(uvMins, uvMaxs, ceilingMaterial->m_spriteCoords);
			AddQuadToIndexedVertexArray(vertices, indices, Vec3(coord, 1), Vec3(coord.x, coord.y + 1, 1), Vec3(coord.x + 1, coord.y + 1, 1), Vec3(coord.x + 1, coord.y, 1), Rgba8::WHITE, uvMins, uvMaxs);
		}
	}
	m_mesh->UpdateIndices(indices);
	m_mesh->UpdateVertices(vertices);
}

RaycastResult TileMap::RaycastOnFloorsAndCeilings(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const
{
	RaycastResult result;
	result.startPosition = startPosition;
	result.forwardNormal = forwardNormal;
	result.maxDistance = maxDistance;
	result.impactDistance = 99999999.f;
	result.impactPosition = startPosition + forwardNormal * maxDistance;
	if (forwardNormal.z > 0.f)
	{
		float scale = (1.f - startPosition.z) / (float)abs(forwardNormal.z);
		if (scale > maxDistance)
		{
			result.didImpact = false;
			return result;
		}
		result.didImpact = true;
		result.impactPosition = startPosition + scale * forwardNormal;
		result.impactDistance = scale;
		result.impactSurfaceNormal = Vec3(0.f, 0.f, -1.f);
	}
	else if (forwardNormal.z < 0.f)
	{
		float scale = startPosition.z / (float)abs(forwardNormal.z);
		if (scale > maxDistance)
		{
			result.didImpact = false;
			return result;
		}
		result.didImpact = true;
		result.impactPosition = startPosition + scale * forwardNormal;
		result.impactDistance = scale;
		result.impactSurfaceNormal = Vec3(0.f, 0.f, 1.f);
	}
	return result;

}

RaycastResult TileMap::RaycastOnEntities(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const
{
	RaycastResult result;
	result.startPosition = startPosition;
	result.forwardNormal = forwardNormal;
	result.maxDistance = maxDistance;
	result.impactDistance = 99999999.f;
	result.impactPosition = startPosition + forwardNormal * maxDistance;
	Vec2 startPosition2D = startPosition.xy();
	Vec2 forwardNormal2D = forwardNormal.xy().GetNormalized();
	float maxDistance2D = GetProjectedLength2D(Vec2(maxDistance, forwardNormal.z), Vec2(1.f, 0.f));

	for (int i = 0; i < m_allEntities.size(); ++i)
	{
		Vec2 entityPosition2D = m_allEntities[i]->m_position;
		float entityRadius = m_allEntities[i]->m_physicsRadius;
		float entityHeight = m_allEntities[i]->m_height;
		if (GetDistance2D(startPosition2D, entityPosition2D) <= maxDistance2D + entityRadius)//check if with in range
		{
			Vec2 nearestPoint = GetNearestPointOnRay2D(entityPosition2D, startPosition2D, startPosition2D + forwardNormal2D);
			if (GetDistance2D(nearestPoint, entityPosition2D) < entityRadius)
			{
				Vec2 pointA, pointB;
				GetIntersectPointsBetweenSegementsAndDisc(startPosition2D, startPosition2D + forwardNormal2D, entityPosition2D, entityRadius, pointA, pointB);
				float scaleA = (pointA.x - startPosition2D.x) / forwardNormal.x;
				float scaleB = (pointB.x - startPosition2D.x) / forwardNormal.x;
				float distanceA = GetDistance2D(pointA, startPosition2D);
				float distanceB = GetDistance2D(pointB, startPosition2D);
				if (distanceA > distanceB)
				{
					if (scaleB * forwardNormal.z + startPosition.z < entityHeight && scaleB * forwardNormal.z + startPosition.z > 0.f)
					{
						result.didImpact = true;
						result.impactPosition = startPosition + scaleB * forwardNormal;
						result.impactEntity = m_allEntities[i];
						result.impactDistance = GetDistance3D(startPosition, result.impactPosition);
						result.impactSurfaceNormal = Vec3((pointB - entityPosition2D).GetNormalized(), 0.f);
						return result;
					}
					else if (scaleA * forwardNormal.z + startPosition.z < entityHeight && scaleA * forwardNormal.z + startPosition.z > 0.f)
					{
						result.didImpact = true;
						float scaleC = (float)abs((startPosition.z - entityHeight) / forwardNormal.z);
						result.impactPosition = startPosition + scaleC * forwardNormal;
						result.impactEntity = m_allEntities[i];
						result.impactDistance = GetDistance3D(startPosition, result.impactPosition);
						result.impactSurfaceNormal = Vec3(0.f, 0.f, 1.f);
						return result;
					}
				}
				else
				{
					if (scaleA * forwardNormal.z + startPosition.z < entityHeight && scaleA * forwardNormal.z + startPosition.z > 0.f)
					{
						result.didImpact = true;
						result.impactPosition = startPosition + scaleA * forwardNormal;
						result.impactEntity = m_allEntities[i];
						result.impactDistance = GetDistance3D(startPosition, result.impactPosition);
						result.impactSurfaceNormal = Vec3((pointA - entityPosition2D).GetNormalized(), 0.f);
						return result;
					}
					else if (scaleB * forwardNormal.z + startPosition.z < entityHeight && scaleB * forwardNormal.z + startPosition.z > 0.f)
					{
						result.didImpact = true;
						float scaleC = (float)abs((startPosition.z - entityHeight) / forwardNormal.z);
						result.impactPosition = startPosition + scaleC * forwardNormal;
						result.impactEntity = m_allEntities[i];
						result.impactDistance = GetDistance3D(startPosition, result.impactPosition);
						result.impactSurfaceNormal = Vec3(0.f, 0.f, 1.f);
						return result;
					}
				}
			}
		}
	}
	return result;
}

RaycastResult TileMap::RaycastOnWalls(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const
{
	RaycastResult result;
	result.startPosition = startPosition;
	result.forwardNormal = forwardNormal;
	result.maxDistance = maxDistance;
	result.impactDistance = 99999999.f;
	result.impactPosition = startPosition + forwardNormal * maxDistance;
	Vec2 startPosition2D = startPosition.xy();
	Vec2 forwardNormal2D = forwardNormal.xy().GetNormalized();
	float maxDistance2D = GetProjectedLength2D(Vec2(maxDistance, forwardNormal.z), Vec2(1.f, 0.f));

	//Compute tile coord
	int tileCoordX = RoundDownToInt(startPosition2D.x);
	int tileCoordY = RoundDownToInt(startPosition2D.y);
	if (tileCoordX < 0 || tileCoordX >= m_dimensions.x || tileCoordY < 0 || tileCoordY >= m_dimensions.y)
	{
		return result;
	}
	//Check if current tile is solid
	if (GetTileByCoords(IntVec2(tileCoordX, tileCoordY)).m_regionType->m_isSolid)
	{
		result.didImpact = true;
		result.impactPosition = startPosition;
		return result;
	}

	Vec2 rayDisplacement = forwardNormal2D * maxDistance2D;

	//X
	float xDeltaT = maxDistance2D / abs(rayDisplacement.x);

	int tileStepX;
	if (rayDisplacement.x >= 0)
	{
		tileStepX = 1;
	}
	else
	{
		tileStepX = -1;
	}
	int offsetToLeadingEdgeX = (tileStepX + 1) / 2;

	float firstVerticalIntersectionX = (float)(tileCoordX + offsetToLeadingEdgeX);
	float tOfNextXCrossing = abs(firstVerticalIntersectionX - startPosition2D.x) * xDeltaT;

	//Y
	float yDeltaT = maxDistance2D / abs(rayDisplacement.y);

	int tileStepY = 0;
	if (rayDisplacement.y >= 0)
	{
		tileStepY = 1;
	}
	else if (rayDisplacement.y < 0)
	{
		tileStepY = -1;
	}
	int offsetToLeadingEdgeY = (tileStepY + 1) / 2;

	float firstVerticalIntersectionY = (float)(tileCoordY + offsetToLeadingEdgeY);
	float tOfNextYCrossing = abs(firstVerticalIntersectionY - startPosition2D.y) * yDeltaT;

	while (true)
	{
		if (tOfNextXCrossing < tOfNextYCrossing)
		{
			if (tOfNextXCrossing > maxDistance2D)
			{
				result.didImpact = false;
				return result;
			}
			tileCoordX += tileStepX;
			if (GetTileByCoords(IntVec2(tileCoordX, tileCoordY)).m_regionType->m_isSolid)
			{
				result.didImpact = true;
				result.impactPosition = startPosition + forwardNormal * tOfNextXCrossing;
				result.impactDistance = GetDistance3D(startPosition, result.impactPosition);
				
				if (tileStepX == 1)
				{
					result.impactSurfaceNormal = Vec3(-1.f, 0.f, 0.f);
				}
				else
				{
					result.impactSurfaceNormal = Vec3(1.f, 0.f, 0.f);
				}
				return result;
			}
			tOfNextXCrossing += xDeltaT;
		}
		else
		{
			if (tOfNextYCrossing > maxDistance2D)
			{
				result.didImpact = false;
				return result;
			}
			tileCoordY += tileStepY;
			if (GetTileByCoords(IntVec2(tileCoordX, tileCoordY)).m_regionType->m_isSolid)
			{
				result.didImpact = true;
				result.impactPosition = startPosition + forwardNormal * tOfNextYCrossing;
				result.impactDistance = GetDistance3D(startPosition, result.impactPosition);

				if (tileStepY == 1)
				{
					result.impactSurfaceNormal = Vec3(0.f, -1.f, 0.f);
				}
				else
				{
					result.impactSurfaceNormal = Vec3(0.f, 1.f, 0.f);
				}
				return result;
			}
			tOfNextYCrossing += yDeltaT;
		}
	}
}

//RaycastResult TileMap::RaycastOnFloorsAndCeilings(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const
//{
// 	Entity* hit = nullptr;
// 	for (int i = 0; i < m_allEntities.size(); ++i)
// 	{
// 		if (GetDistance3D(Vec3(m_allEntities[i]->m_position, 0.f), startPosition) > maxDistance + m_allEntities[i]->m_physicsRadius)
// 		{
// 			Vec2 nearestPoint = GetNearestPointOnLineSegment2D(m_allEntities[i]->m_position, startPosition.xy(), startPosition.xy() + forwardNormal.xy());
// 			if (GetDistance3D(Vec3(nearestPoint, 0.f), startPosition) < maxDistance)
// 			{
// 				Vec2 pointA, pointB;
// 				GetIntersectPointsBetweenSegementsAndDisc(startPosition.xy(), startPosition.xy() + forwardNormal.xy(), m_allEntities[i]->m_position, m_allEntities[i]->m_physicsRadius, pointA, pointB);
// 				if()
// 			}
// 		}
// 	}
//}

RaycastResult TileMap::Raycast(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const
{
	RaycastResult resultOnWall = RaycastOnWalls(startPosition, forwardNormal, maxDistance);
	RaycastResult resultOnFloorAndCeiling = RaycastOnFloorsAndCeilings(startPosition, forwardNormal, maxDistance);
	RaycastResult resultOnEntities = RaycastOnEntities(startPosition, forwardNormal, maxDistance);
	if (resultOnWall.impactDistance < resultOnFloorAndCeiling.impactDistance && resultOnWall.impactDistance < resultOnEntities.impactDistance)
	{
		return resultOnWall;
	}
	else if (resultOnEntities.impactDistance < resultOnFloorAndCeiling.impactDistance && resultOnEntities.impactDistance < resultOnWall.impactDistance)
	{
		return resultOnEntities;
	}
	else
	{
		return resultOnFloorAndCeiling;
	}
}

// void TileMap::GenerateTiles()
// {
// 	for (int tileIndex = 0; tileIndex < m_dimensions.x * m_dimensions.y; ++tileIndex)
// 	{
// 		IntVec2 coordinate = GetTileCoordsForTileIndex(tileIndex);
// 		Tile tile(coordinate, MapRe);
// 		if (coordinate.x == 0 || coordinate.y == 0 || coordinate.x == m_dimensions.x - 1 || coordinate.y == m_dimensions.y - 1)
// 		{
// 			//tile.m_type = TERRAIN_TYPE_STONE;
// 			tile.m_templsSolid = true;
// 		}
// 		else if (!(coordinate.x >= 1 && coordinate.x <= 5 && coordinate.y >= 1 && coordinate.y <= 5)
// 			&& !(coordinate.x >= m_dimensions.x - 6 && coordinate.x <= m_dimensions.x - 2 && coordinate.y >= m_dimensions.y - 6 && coordinate.y <= m_dimensions.y - 2))
// 		{
// 			int random = g_theRng->RollRandomIntInRange(0, 9);
// 			if (random < 2) {
// 				//tile.m_type = TERRAIN_TYPE_STONE;
// 				tile.m_templsSolid = true;
// 			}
// 		}
// 		m_tiles.push_back(tile);
// 	}
// }

const IntVec2 TileMap::GetTileCoordsForTileIndex(int tileIndex) const
{
	int tileY = tileIndex / m_dimensions.x;
	int tileX = tileIndex - tileY * m_dimensions.x;
	return IntVec2(tileX, tileY);
}

Tile TileMap::GetTileByCoords(IntVec2 coord) const
{
	int index = (m_dimensions.y - coord.y - 1) * m_dimensions.x + coord.x;
	return m_tiles[index];
}

void TileMap::HandleEntitiesAgainstWall(Entity* entity)
{
	if (!entity->m_canBePushedByWalls)
	{
		return;
	}
	int entityCoordX = RoundDownToInt(entity->m_position.x);
	int entityCoordY = RoundDownToInt(entity->m_position.y);
	if (GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX, entityCoordY + 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX, entityCoordY + 1)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX, entityCoordY - 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX, entityCoordY - 1)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY + 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY + 1)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY - 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX + 1, entityCoordY - 1)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY + 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY + 1)).GetBound());
	}

	if (GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY - 1)).m_regionType->m_isSolid)
	{
		PushDiscOutOfAABB2D(entity->m_position, entity->m_physicsRadius, GetTileByCoords(IntVec2(entityCoordX - 1, entityCoordY - 1)).GetBound());
	}
}