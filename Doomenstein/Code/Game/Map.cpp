#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Entity.hpp"
#include "Game/Actor.hpp"
#include "Game/Portal.hpp"
#include "Game/Projectile.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Map::~Map()
{
	for (int i = 0; i < m_allEntities.size(); ++i)
	{
		delete m_allEntities[i];
	}
	m_allEntities.clear();
	m_projectiles.clear();
	m_actors.clear();
}

void Map::Update(float deltaSeconds)
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); ++entityIndex)
	{
		m_allEntities[entityIndex]->Update(deltaSeconds);
		for (int entityIndexJ = entityIndex + 1; entityIndexJ < m_allEntities.size(); ++entityIndexJ)
		{
			if (m_allEntities[entityIndex]->m_canBePushedByEntities && m_allEntities[entityIndexJ]->m_canBePushedByEntities)//other wise nothing happened
			{
				if (m_allEntities[entityIndex]->m_canPushEntities && m_allEntities[entityIndex]->m_canPushEntities)
				{
					PushDiscsOutOfEachOther2D(m_allEntities[entityIndex]->m_position, m_allEntities[entityIndex]->m_physicsRadius,
						m_allEntities[entityIndexJ]->m_position, m_allEntities[entityIndexJ]->m_physicsRadius, 
						m_allEntities[entityIndex]->m_mass / (m_allEntities[entityIndex]->m_mass + m_allEntities[entityIndexJ]->m_mass));
				}
				else if (!m_allEntities[entityIndex]->m_canPushEntities && m_allEntities[entityIndex]->m_canPushEntities)
				{
					PushDiscOutOfDisc2D(m_allEntities[entityIndex]->m_position, m_allEntities[entityIndex]->m_physicsRadius,
						m_allEntities[entityIndexJ]->m_position, m_allEntities[entityIndexJ]->m_physicsRadius);
				}
				else if (m_allEntities[entityIndex]->m_canPushEntities && !m_allEntities[entityIndex]->m_canPushEntities)
				{
					PushDiscOutOfDisc2D(m_allEntities[entityIndexJ]->m_position, m_allEntities[entityIndexJ]->m_physicsRadius,
						m_allEntities[entityIndex]->m_position, m_allEntities[entityIndex]->m_physicsRadius);
				}
			}
		}
	}
}

void Map::RenderDebug() const
{
	GPUMesh debugMesh(g_theRenderer);
	std::vector<unsigned int> indices;
	std::vector<Vertex_PCU> vertices;
	for (int i = 0; i < m_allEntities.size(); ++i)
	{
		m_allEntities[i]->AddDebugToVertexArray(vertices, indices);
	}
	debugMesh.UpdateIndices(indices);
	debugMesh.UpdateVertices(vertices);
	g_theRenderer->SetFillMode(WIREFRAME);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawMesh(&debugMesh);
}

Entity* Map::SpawnNewEntityOfType(const std::string& entityDefName)
{
	if (EntityDefinition::s_definitions[entityDefName])
	{
		return SpawnNewEntityOfType(*EntityDefinition::s_definitions[entityDefName]);
	}
	return nullptr;
}

Entity* Map::SpawnNewEntityOfType(const EntityDefinition& entityDef)
{
	Entity* newEntity = nullptr;
	if (entityDef.m_className == "Actor")
	{
		newEntity = new Actor(entityDef, this);
		m_actors.push_back(newEntity);
	}
	else if (entityDef.m_className == "Projectile")
	{
		newEntity = new Projectile(entityDef, this);
		m_projectiles.push_back(newEntity);
	}
	else if (entityDef.m_className == "Portal")
	{
		newEntity = new Portal(entityDef, this);
	}
	else if (entityDef.m_className == "Entity")
	{
		newEntity = new Entity(entityDef, this);
	}
	if (newEntity)
	{
		m_allEntities.push_back(newEntity);
	}
	return newEntity;
}

void Map::AddEntityToMap(const std::string& entityDefName, Vec2 pos, float yaw)
{
	Entity* newEntity = SpawnNewEntityOfType(entityDefName);
	newEntity->m_position = pos;
	newEntity->m_orientationDegrees = yaw;
}

Entity* Map::GetEntityCanBePossessed(const Camera& camera)
{
	Entity* target = nullptr;
	float nearestDistance = 2.f;
	for (int i = 0; i < m_allEntities.size(); ++i)
	{
		if (IsPointInForwardSector2D(m_allEntities[i]->m_position, Vec2(camera.GetPosition().x, camera.GetPosition().y), camera.GetCameraYaw(), 90.f, 2.f))
		{
			float distance = GetDistance3D(Vec3(m_allEntities[i]->m_position, 0.f), camera.GetPosition());
			if (distance < nearestDistance && m_allEntities[i]->m_className != "Portal")
			{
				target = m_allEntities[i];
				nearestDistance = distance;
			}
		}
	}
	return target;
}
