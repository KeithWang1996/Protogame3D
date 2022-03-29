#include "Game/Portal.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>
Portal::Portal(const EntityDefinition& def, Map* map)
	: Entity(def, map)
{
	m_size = def.m_size;
	m_billboardType = def.m_billboardType;
	m_idleAnim = def.m_idleAnim;
}

void Portal::Render(const Camera& camera) const
{
	if (!m_idleAnim)
	{
		return;
	}
	GPUMesh portalMesh(g_theRenderer);
	std::vector<Vertex_PCU> vertices;
	std::vector<unsigned int> indices;
	//Vec3 topLeft, topRight, bottomLeft, bottomRight;
	Vec2 uvMaxs, uvMins;
	Vec3 up, forward, left;
	Vec3 cameraPosition = camera.GetPosition();
	Vec3 cameraForward = camera.GetForward();
	Vec3 cameraUp = camera.GetUp();
	Vec3 center = Vec3(m_position, m_size.y * 0.5f);
	//Process
	switch (m_billboardType)
	{
	case CAMERA_FACING_XY:
	{
		up = Vec3(0.f, 0.f, 1.f);
		Vec2 diff = cameraPosition.xy() - m_position;
		forward = Vec3(diff.GetNormalized(), 0.f);
		left = Vec3(forward.xy().GetRotatedMinus90Degrees(), 0.f);
		break;
	}
	case CAMERA_OPPOSING_XY:
	{
		up = Vec3(0.f, 0.f, 1.f);
		forward = Vec3(-cameraForward.xy(), 0.f);
		left = Vec3(forward.xy().GetRotatedMinus90Degrees(), 0.f);
		break;
	}
	case CAMERA_FACING_XYZ:
	{
		forward = (cameraPosition - center).GetNormalized();
		left = CrossProduct3D(forward, Vec3(0.f, 0.f, 1.f)).GetNormalized();
		up = CrossProduct3D(left, forward);
		break;
	}
	case CAMERA_OPPOSING_XYZ:
	{
		up = cameraUp;
		forward = -cameraForward;
		left = CrossProduct3D(forward, up);
		break;
	}
	default:
		break;
	}
	Vec3 bottomLeft, bottomRight, topRight, topLeft;
	bottomLeft = center + left * m_size.x * 0.5f - up * m_size.y * 0.5f;
	bottomRight = center - left * m_size.x * 0.5f - up * m_size.y * 0.5f;
	topLeft = center + left * m_size.x * 0.5f + up * m_size.y * 0.5f;
	topRight = center - left * m_size.x * 0.5f + up * m_size.y * 0.5f;
	m_idleAnim->GetAnimDefinition("coords")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
	AddQuadToIndexedVertexArray(vertices, indices, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, uvMins, uvMaxs);
	portalMesh.UpdateIndices(indices);
	portalMesh.UpdateVertices(vertices);
	g_theRenderer->BindTexture(&m_idleAnim->m_spriteSheet->GetTexture());
	//g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawMesh(&portalMesh);
}

void Portal::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	std::vector<Entity*>& entities = m_map->m_allEntities;
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i]->m_className != "Portal" && DoDiscsOverlap(m_position, m_physicsRadius, entities[i]->m_position, entities[i]->m_physicsRadius))
		{
			if (m_map->m_name == m_destMap || m_destMap == "")
			{
				entities[i]->m_position = m_destPos;
				entities[i]->m_orientationDegrees += m_destYawOffset;
			}
			else if(g_theGame->GetWorld()->GetMap(m_destMap) && g_theGame->GetPlayer() == entities[i])
			{
				entities[i]->m_position = m_destPos;
				entities[i]->m_orientationDegrees += m_destYawOffset;
				g_theGame->GetWorld()->MoveEntityToAnotherMap(entities[i], m_destMap);
				g_theGame->GetWorld()->SetCurrentMap(m_destMap);
			}
		}
	}
}