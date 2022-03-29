#pragma once
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
struct IntVec2;
class GPUMesh;
class TileMap : public Map
{
public:
	TileMap(int sizeX, int sizeY);
	TileMap(const tinyxml2::XMLElement& mapElement);
	~TileMap();
	virtual void Render(const Camera& camera) const override;
	virtual void Update(float deltaSeconds) override;
	virtual void UpdateMeshes() override;
	virtual RaycastResult Raycast(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const override;
	//Three helper function for raycast;
	RaycastResult RaycastOnWalls(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const;
	RaycastResult RaycastOnFloorsAndCeilings(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const;
	RaycastResult RaycastOnEntities(Vec3 startPosition, Vec3 forwardNormal, float maxDistance) const;
	//void GenerateTiles();
	const IntVec2 GetTileCoordsForTileIndex(int tileIndex) const;
	Tile GetTileByCoords(IntVec2 coord) const;
	void HandleEntitiesAgainstWall(Entity* entity);
private:
	IntVec2 m_dimensions;
	std::vector<Tile> m_tiles;
	GPUMesh* m_mesh = nullptr;
};