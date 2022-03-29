#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

Tile::Tile(IntVec2 position, MapRegionType* regionType)
	: m_position(position)
	, m_regionType(regionType)
{}

void Tile::Render() const
{
	AABB2 aabb = GetBound();
	Rgba8 tint = Rgba8(100, 255, 100, 255);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2(aabb, tint);
}

AABB2 Tile::GetBound() const
{
	return AABB2(Vec2(static_cast<float>(m_position.x), static_cast<float>(m_position.y)), Vec2(static_cast<float>(m_position.x + 1.f), static_cast<float>(m_position.y + 1.f)));
}

std::map<std::string, MapMaterialType*> MapMaterialType::s_definitions;
std::map<std::string, SpriteSheet*> MapMaterialType::s_spriteSheet;//clear at ~World()
MapMaterialType* MapMaterialType::errorMaterial;
SpriteSheet* MapMaterialType::errorSheet;

MapMaterialType::MapMaterialType(const tinyxml2::XMLElement& materialDefinitionElement)
{
	m_name = ParseXmlAttribute(materialDefinitionElement, "name", "");
	if (m_name == "")
	{
		g_theConsole->Error("Material Name not found");
	}
	std::string sheetName = ParseXmlAttribute(materialDefinitionElement, "sheet", "");
	if (sheetName == "")
	{
		g_theConsole->Error("Sheet Name not found");
		m_sheet = s_spriteSheet["TestTerrain"];
	}
	else
	{
		m_sheet = s_spriteSheet[sheetName];
	}
	m_spriteCoords = ParseXmlAttribute(materialDefinitionElement, "spriteCoords", IntVec2(-1, -1));
	if (m_spriteCoords == IntVec2(-1, -1))
	{
		g_theConsole->Error("Sprite coordinate of " + m_name + " not found");
		m_spriteCoords = IntVec2(0, 0);
	}
	IntVec2 sheetLayout = m_sheet->GetLayout();
	if (m_spriteCoords.x >= sheetLayout.x || m_spriteCoords.y >= sheetLayout.y)
	{
		g_theConsole->Error("Sprite coordinate of " + m_name + " out of bounds");
		m_spriteCoords = IntVec2(0, 0);
	}
}

void MapMaterialType::InitializeMaterialDefinition(const tinyxml2::XMLElement& materialDefinitionsElement)
{
	errorSheet = new SpriteSheet(*g_theRenderer->m_defaultTexture, IntVec2(1,1));
	errorMaterial = new MapMaterialType();
	errorMaterial->m_name = "Error";
	errorMaterial->m_sheet = errorSheet;
	errorMaterial->m_spriteCoords = IntVec2(0, 0);
	//Hard code loading sheet
	const tinyxml2::XMLElement* sheetDefElement = materialDefinitionsElement.FirstChildElement("MaterialsSheet");
	if (sheetDefElement)
	{
		std::string sheetName = ParseXmlAttribute(*sheetDefElement, "name", "");
		IntVec2 layout = ParseXmlAttribute(*sheetDefElement, "layout", IntVec2::ZERO);
		const tinyxml2::XMLElement* diffuseDefElement = sheetDefElement->FirstChildElement();
		std::string filePath = "";
		if (!diffuseDefElement)
		{
			g_theConsole->Error("Material sheet diffuse path did not found. Using default path");
			filePath = "Data/Images/Terrain_8x8.png";
		}
		else
		{
			filePath = ParseXmlAttribute(*diffuseDefElement, "image", "");
		}
		
		Texture* sheetTexture = g_theRenderer->CreateOrGetTextureFromFile(filePath.c_str());
		s_spriteSheet[sheetName] = new SpriteSheet(*sheetTexture, layout);
	}
	else
	{
		Texture* sheetTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Terrain_8x8.png");
		s_spriteSheet["TestTerrain"] = new SpriteSheet(*sheetTexture, IntVec2());
		g_theConsole->Error("Material sheet did not found. Using Default sheet");
	}
	

	//loading material definition
	const tinyxml2::XMLElement* materialDefElement = materialDefinitionsElement.FirstChildElement("MaterialType");
	while (materialDefElement)
	{
		MapMaterialType* newMapMaterialType = new MapMaterialType(*materialDefElement);
		s_definitions[newMapMaterialType->m_name] = newMapMaterialType;
		materialDefElement = materialDefElement->NextSiblingElement();
		g_theConsole->PrintString(Rgba8(200, 200, 200, 255), "Material type " + newMapMaterialType->m_name + " is loaded.");
	}
	g_theConsole->PrintString(Rgba8(0, 255, 0, 255), "Material loading succeed.");
}


std::map<std::string, MapRegionType*> MapRegionType::s_definitions;
MapRegionType* MapRegionType::errorRegion;

MapRegionType::MapRegionType(const tinyxml2::XMLElement& regionDefinitionElement)
{
	m_name = ParseXmlAttribute(regionDefinitionElement, "name", "");
	if (m_name == "")
	{
		g_theConsole->Error("Region Name not found");
	}
	m_isSolid = ParseXmlAttribute(regionDefinitionElement, "isSolid", false);
	if (m_isSolid)
	{
		const tinyxml2::XMLElement* sideElement = regionDefinitionElement.FirstChildElement("Side");
		std::string materialSideName = ParseXmlAttribute(*sideElement, "material", "");
		m_sideMaterial = MapMaterialType::s_definitions[materialSideName];
		if (!m_sideMaterial)
		{
			g_theConsole->Error("Floor material " + materialSideName + " is invalid, using error material");
			m_sideMaterial = MapMaterialType::errorMaterial;
		}
	}
	else
	{
		const tinyxml2::XMLElement* floorElement = regionDefinitionElement.FirstChildElement("Floor");
		std::string materialFloorName = ParseXmlAttribute(*floorElement, "material", "");
		m_floorMaterial = MapMaterialType::s_definitions[materialFloorName];
		if (!m_floorMaterial)
		{
			g_theConsole->Error("Floor material " + materialFloorName + " is invalid, using error material");
			m_floorMaterial = MapMaterialType::errorMaterial;
		}
		const tinyxml2::XMLElement* ceilingElement = regionDefinitionElement.FirstChildElement("Ceiling");
		std::string materialCeilingName = ParseXmlAttribute(*ceilingElement, "material", "");
		m_ceilingMaterial = MapMaterialType::s_definitions[materialCeilingName];
		if (!m_ceilingMaterial)
		{
			g_theConsole->Error("Floor material " + materialCeilingName + " is invalid, using error material");
			m_ceilingMaterial = MapMaterialType::errorMaterial;
		}
	}
}

void MapRegionType::InitializeRegionDefinition(const tinyxml2::XMLElement& regionDefinitionsElement)
{
	errorRegion = new MapRegionType();
	errorRegion->m_isSolid = true;
	errorRegion->m_sideMaterial = MapMaterialType::errorMaterial;
	const tinyxml2::XMLElement* regionDefElement = regionDefinitionsElement.FirstChildElement();
	while (regionDefElement)
	{
		MapRegionType* newMapRegionType = new MapRegionType(*regionDefElement);
		s_definitions[newMapRegionType->m_name] = newMapRegionType;
		regionDefElement = regionDefElement->NextSiblingElement();
		g_theConsole->PrintString(Rgba8(200, 200, 200, 255), "Region type " + newMapRegionType->m_name + " is loaded.");
	}
	g_theConsole->PrintString(Rgba8(0, 255, 0, 255), "Region loading succeed.");
}