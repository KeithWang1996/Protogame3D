#pragma once
#include "Engine/Math/Vec2.hpp"
class Map;
class Player;
class World
{
public:
	void Update(float deltaSeconds);
	World();
	~World();
	void Render() const;
	void RenderDebug() const;
	void AddPlayer(Player* player);
	const Vec2 GetPlayerPosition() const;
private:
	Map* m_currentMap;
};