#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "GameCommon.hpp"

World::World()
{
	m_currentMap = new Map(static_cast<int>(WORLD_SIZE_X), static_cast<int>(WORLD_SIZE_Y));
}

World::~World()
{
	delete m_currentMap;
	m_currentMap = nullptr;
}
void World::Render() const
{
	m_currentMap->Render();
}

void World::AddPlayer(Player* player)
{
	m_currentMap->AddEntity(player);
}

void World::Update(float deltaSeconds)
{
	m_currentMap->Update(deltaSeconds);
}

const Vec2 World::GetPlayerPosition() const
{
	return m_currentMap->GetPlayerPosition();
}

void World::RenderDebug() const
{
	m_currentMap->RenderDebug();
}

