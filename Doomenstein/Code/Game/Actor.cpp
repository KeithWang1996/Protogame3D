#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
Actor::Actor(const EntityDefinition& def, Map* map)
	: Entity(def, map)
{
	m_eyeHeight = def.m_eyeHeight;
	m_walkSpeed = def.m_speed;
	m_size = def.m_size;
	m_billboardType = def.m_billboardType;
	m_walkAnim = def.m_walkAnim;
	m_attackAnim = def.m_attackAnim;
	m_painAnim = def.m_painAnim;
	m_deathAnim = def.m_deathAnim;
}

void Actor::Render(const Camera& camera) const
{
	GPUMesh actorMesh(g_theRenderer);
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
	
	float cameraRelativeDegrees = (cameraPosition.xy() - m_position).GetAngleDegrees() - m_orientationDegrees;
	//Correct angle
	while (cameraRelativeDegrees < 0.f)
	{
		cameraRelativeDegrees += 360.f;
	}
	while (cameraRelativeDegrees > 360.f)
	{
		cameraRelativeDegrees -= 360.f;
	}
	if (m_walkAnim->GetSize() == 4)
	{
		if (cameraRelativeDegrees >= 315.f || cameraRelativeDegrees < 45.f)
		{
			m_walkAnim->GetAnimDefinition("front")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 45.f && cameraRelativeDegrees < 135.f)
		{
			m_walkAnim->GetAnimDefinition("left")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 135.f && cameraRelativeDegrees < 225.f)
		{
			m_walkAnim->GetAnimDefinition("back")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else
		{
			m_walkAnim->GetAnimDefinition("right")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
	}
	else if (m_walkAnim->GetSize() == 8)
	{
		if (cameraRelativeDegrees >= 337.5f || cameraRelativeDegrees < 22.5f)
		{
			m_walkAnim->GetAnimDefinition("front")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 22.5f && cameraRelativeDegrees < 67.5f)
		{
			m_walkAnim->GetAnimDefinition("frontLeft")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 67.5f && cameraRelativeDegrees < 112.5f)
		{
			m_walkAnim->GetAnimDefinition("left")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 112.5f && cameraRelativeDegrees < 157.5f)
		{
			m_walkAnim->GetAnimDefinition("backLeft")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 157.5 && cameraRelativeDegrees < 202.5f)
		{
			m_walkAnim->GetAnimDefinition("back")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 202.5f && cameraRelativeDegrees < 247.5f)
		{
			m_walkAnim->GetAnimDefinition("backRight")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else if (cameraRelativeDegrees >= 247.5f && cameraRelativeDegrees < 292.5f)
		{
			m_walkAnim->GetAnimDefinition("right")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
		else 
		{
			m_walkAnim->GetAnimDefinition("frontRight")->GetSpriteDefAtTime(0.f).GetUVs(uvMins, uvMaxs);
		}
	}
	
	AddQuadToIndexedVertexArray(vertices, indices, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, uvMins, uvMaxs);
	actorMesh.UpdateIndices(indices);
	actorMesh.UpdateVertices(vertices);
	g_theRenderer->BindTexture(&m_walkAnim->m_spriteSheet->GetTexture());
	//g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawMesh(&actorMesh);
}