#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "GameCommon.hpp"
#include "Game/TileMap.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Entity.hpp"
#include <vector>
World::World()
{
	//Parsing Entities
	tinyxml2::XMLDocument entityDefDoc;
	entityDefDoc.LoadFile("Data/Definitions/EntityTypes.xml");
	g_theConsole->PrintString(Rgba8::WHITE, "Loading entities...");
	if (entityDefDoc.ErrorID() != 0)
	{
		g_theConsole->Error("Fatal Error: Missing EntityTypes.xml");
		return;
	}
	else
	{
		std::string rootName = entityDefDoc.RootElement()->Name();
		if (rootName != "EntityTypes")
		{
			g_theConsole->Error("EntityTypes.xml has wrong root node name.");
		}
		//g_theConsole->Error("MapMaterialTypes.xml has wrong root node name.");
		EntityDefinition::InitializeEntityDefinitions(*entityDefDoc.RootElement());
	}
	//Parsing Materials
	tinyxml2::XMLDocument materialDefDoc;
	materialDefDoc.LoadFile("Data/Definitions/MapMaterialTypes.xml");
	g_theConsole->PrintString(Rgba8::WHITE, "Loading material types...");
	//GUARANTEE_OR_DIE(materialDefDoc.ErrorID() == 0, "Material load failed");
	if (materialDefDoc.ErrorID() != 0)
	{
		g_theConsole->Error("Fatal Error: Missing MapMaterialTypes.xml");
		return;
		//Create default error definitions
	}
	else
	{	
		std::string rootName = materialDefDoc.RootElement()->Name();
		if (rootName != "MapMaterialTypes")
		{
			g_theConsole->Error("MapMaterialTypes.xml has wrong root node name.");
		}
		//g_theConsole->Error("MapMaterialTypes.xml has wrong root node name.");
		MapMaterialType::InitializeMaterialDefinition(*materialDefDoc.RootElement());
	}

	//Parsing Regions
	tinyxml2::XMLDocument regionDefDoc;
	regionDefDoc.LoadFile("Data/Definitions/MapRegionTypes.xml");
	g_theConsole->PrintString(Rgba8::WHITE, "Loading region types...");
	if (regionDefDoc.ErrorID() != 0)
	{
		g_theConsole->Error("Fatal Error: Missing MapMaterialTypes.xml");
		return;
	}
	else
	{
		std::string rootName = regionDefDoc.RootElement()->Name();
		if (rootName != "MapRegionTypes")
		{
			g_theConsole->Error("MapRegionTypes.xml has wrong root node name.");
		}
		MapRegionType::InitializeRegionDefinition(*regionDefDoc.RootElement());
	}

	LoadAllMaps("Data/Maps/", "*.xml");
	m_currentMap = m_maps["TestRoom"];

	//test entities
	//m_currentMap->AddEntityToMap("Marine", Vec2(4,3), 90.f);
}

World::~World()
{
	//delete m_currentMap;
	m_currentMap = nullptr;

	delete MapMaterialType::errorMaterial;
	MapMaterialType::errorMaterial = nullptr;
	delete MapMaterialType::errorSheet;
	MapMaterialType::errorSheet = nullptr;
	delete MapRegionType::errorRegion;
	MapRegionType::errorRegion = nullptr;

	std::map<std::string, SpriteSheet*>::iterator itrSheet;
	for (itrSheet = MapMaterialType::s_spriteSheet.begin(); itrSheet != MapMaterialType::s_spriteSheet.end(); ++itrSheet)
	{
		delete itrSheet->second;
	}
	MapMaterialType::s_spriteSheet.clear();

	std::map<std::string, MapMaterialType*>::iterator itrMaterial;
	for (itrMaterial = MapMaterialType::s_definitions.begin(); itrMaterial != MapMaterialType::s_definitions.end(); ++itrMaterial)
	{
		delete itrMaterial->second;
	}
	MapMaterialType::s_definitions.clear();

	std::map<std::string, MapRegionType*>::iterator itrRegion;
	for (itrRegion = MapRegionType::s_definitions.begin(); itrRegion != MapRegionType::s_definitions.end(); ++itrRegion)
	{
		delete itrRegion->second;
	}
	MapRegionType::s_definitions.clear();

	std::map<std::string, Map*>::iterator itrMap;
	for (itrMap = m_maps.begin(); itrMap != m_maps.end(); ++itrMap)
	{
		delete itrMap->second;
	}
	m_maps.clear();

	std::map<std::string, EntityDefinition*>::iterator itrEntity;
	for (itrEntity = EntityDefinition::s_definitions.begin(); itrEntity != EntityDefinition::s_definitions.end(); ++itrEntity)
	{
		delete itrEntity->second;
	}
	EntityDefinition::s_definitions.clear();
}
void World::Render(const Camera& camera) const
{
	m_currentMap->Render(camera);
	if (g_debugDraw)
	{
		m_currentMap->RenderDebug();
	}
}

void World::AddPlayer(Player* player)
{
	UNUSED(player);
	//m_currentMap->AddEntity(player);
}

void World::MoveEntityToAnotherMap(Entity* entity, std::string mapName)
{
	std::vector<Entity*>& allEntities = m_currentMap->m_allEntities;
	std::vector<Entity*>& actorEntities = m_currentMap->m_actors;
	for (int i = 0; i < allEntities.size(); ++i)
	{
		if (allEntities[i] == entity)
		{
			allEntities.erase(allEntities.begin() + i);
		}
	}
	for (int i = 0; i < actorEntities.size(); ++i)
	{
		if (actorEntities[i] == entity)
		{
			actorEntities.erase(actorEntities.begin() + i);
		}
	}

	Map* targetMap = m_maps[mapName];

	targetMap->m_allEntities.push_back(entity);
	if (entity->m_className == "Actor")
	{
		targetMap->m_actors.push_back(entity);
	}
	entity->m_position = targetMap->m_playerStart;
}

void World::Update(float deltaSeconds)
{
	m_currentMap->Update(deltaSeconds);
}

const Vec2 World::GetPlayerPosition() const
{
	//return m_currentMap->GetPlayerPosition();
	return Vec2::ZERO;
}

void World::LoadAllMaps(std::string path, std::string format)
{
	Strings mapNames = ReadAllFilesIn(path + format);
	for (int i = 0; i < mapNames.size(); ++i)
	{
		//Load one map xml
		std::string mapPath = path + mapNames[i];
		tinyxml2::XMLDocument mapDefDoc;
		mapDefDoc.LoadFile(mapPath.c_str());
		GUARANTEE_OR_DIE(mapDefDoc.ErrorID() == 0, "Map load failed");

		tinyxml2::XMLElement* mapRootElement = mapDefDoc.RootElement();
		std::string rootName = mapRootElement->Name();
		if (rootName != "MapDefinition")
		{
			g_theConsole->Error("Map has wrong root node name.");
		}
		//Read Attribute type
		std::string typeName = ParseXmlAttribute(*mapRootElement, "type", "");
		if (typeName == "TileMap")
		{
			TileMap* tilemap = new TileMap(*mapRootElement);
			Strings mapName = SplitStringOnDelimiter(mapNames[i], '.');
			tilemap->m_name = mapName[0];
			m_maps[mapName[0]] = tilemap;
		}
	}

}

void World::RenderDebug() const
{
	//m_currentMap->RenderDebug();
}

void World::SetCurrentMap(std::string mapName)
{
	g_theConsole->PrintString(Rgba8(0, 0, 255, 255), "Entering " + mapName);
	if (m_maps[mapName])
	{
		m_currentMap = m_maps[mapName];
	}
}

Strings World::GetAllMapNames()
{
	Strings mapNames;
	for (auto itr = m_maps.begin(); itr != m_maps.end(); ++itr)
	{
		mapNames.push_back(itr->first);
	}
	return mapNames;
}

