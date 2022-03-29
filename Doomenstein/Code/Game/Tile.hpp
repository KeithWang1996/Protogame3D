#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
class SpriteSheet;
class MapMaterialType
{
public:
	static std::map<std::string, MapMaterialType*> s_definitions;
	static std::map<std::string, SpriteSheet*> s_spriteSheet;
	std::string m_name = "";
	SpriteSheet* m_sheet = nullptr;
	IntVec2 m_spriteCoords;
public:
	MapMaterialType() = default;
	explicit MapMaterialType(const tinyxml2::XMLElement& materialDefinitionElement);
	static void InitializeMaterialDefinition(const tinyxml2::XMLElement& materialDefinitionsElement);
	static MapMaterialType* errorMaterial;
	static SpriteSheet* errorSheet;
};

class MapRegionType
{
public:
	static std::map<std::string, MapRegionType*> s_definitions;
	std::string m_name = "";
	bool m_isSolid = false;
	MapMaterialType* m_sideMaterial = nullptr;
	MapMaterialType* m_floorMaterial = nullptr;
	MapMaterialType* m_ceilingMaterial = nullptr;
public:
	MapRegionType() = default;
	explicit MapRegionType(const tinyxml2::XMLElement& regionDefinitionElement);
	static void InitializeRegionDefinition(const tinyxml2::XMLElement& regionDefinitionsElement);
	static MapRegionType* errorRegion;
};

enum TileType
{
	TERRAIN_TYPE_GRASS,
	TERRAIN_TYPE_STONE
};
class Tile
{
public:
	Tile(IntVec2 position, MapRegionType* regionType);
	//explicit Tile(int tileX, int tileY);
	void Render() const;
	AABB2 GetBound() const;

	IntVec2 m_position;
	//bool m_templsSolid = false;
	MapRegionType* m_regionType;
	//TileType m_type = TERRAIN_TYPE_GRASS;
};