#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <map>
#include <string>
class Map;
class Player;
class Entity;
class World
{
public:
	void Update(float deltaSeconds);
	World();
	~World();
	void Render(const Camera& camera) const;
	void RenderDebug() const;
	void AddPlayer(Player* player);
	void MoveEntityToAnotherMap(Entity* entity, std::string mapName);
	const Vec2 GetPlayerPosition() const;
	void LoadAllMaps(std::string path, std::string format);
	Map* GetCurrentMap() { return m_currentMap; }
	Map* GetMap(std::string name) { return m_maps[name]; }
	void SetCurrentMap(std::string mapName);

	Strings GetAllMapNames();
private:
	Map* m_currentMap = nullptr;
	std::map<std::string, Map*> m_maps;
};