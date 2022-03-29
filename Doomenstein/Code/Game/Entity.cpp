#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Game/Map.hpp"

Entity::Entity(const EntityDefinition& def, Map* map)
	: m_map(map)
{
	m_physicsRadius = def.m_radius;
	m_height = def.m_height;
	m_className = def.m_className;
}

void Entity::Update(float deltaSeconds)
{
	m_position += deltaSeconds * m_velocity;
	m_orientationDegrees += deltaSeconds * m_angularVelocity;
}

void Entity::Render(const Camera& camera) const
{
	UNUSED(camera);
	//Todo: Add Entity render
}

void Entity::Die()
{
	m_isDead = true;
}

Vec2 Entity::GetForwardVector()
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees, 1.f);
}

bool Entity::IsAlive() const
{
	if (m_isDead)
	{
		return false;
	}
	return true;
}

void Entity::RenderDebug() const
{
	g_theRenderer->BindTexture(nullptr);
}

void Entity::AddDebugToVertexArray(std::vector<Vertex_PCU>& vertices, std::vector<unsigned int>& indices) const
{
	//Test
	AddCylinderToIndexedVertexArray(vertices, indices, Vec3(m_position, 0.f), Vec3(m_position, m_height), m_physicsRadius, Rgba8::WHITE, Rgba8::WHITE);
}

void Entity::RotateYawDegrees(float degrees)
{
	m_orientationDegrees = (float)fmod(m_orientationDegrees + degrees + 180.f, 360.f) - 180.f;
}

//definition functions
std::map<std::string, EntityDefinition*> EntityDefinition::s_definitions;

EntityDefinition::EntityDefinition(const tinyxml2::XMLElement& entityElement)
{
	m_className = entityElement.Name();
	m_name = ParseXmlAttribute(entityElement, "name", "NameError");
	if (m_name == "NameError")
	{
		g_theConsole->Error("Cannot find name for an entity definition");
	}
	const tinyxml2::XMLElement* physicsElement = entityElement.FirstChildElement("Physics");
	if (physicsElement)//if exist, load
	{
		m_radius = ParseXmlAttribute(*physicsElement, "radius", 0.f);
		m_height = ParseXmlAttribute(*physicsElement, "height", 0.f);
		m_eyeHeight = ParseXmlAttribute(*physicsElement, "eyeHeight", 0.f);
		m_speed = ParseXmlAttribute(*physicsElement, "walkSpeed", -1.f);
		if (m_speed == -1.f)//if not actor
		{
			m_speed = ParseXmlAttribute(*physicsElement, "walkSpeed", -1.f);
		}
	}
	const tinyxml2::XMLElement* appearenceElement = entityElement.FirstChildElement("Appearance");
	if (appearenceElement)
	{
		m_size = ParseXmlAttribute(*appearenceElement, "size", Vec2::ZERO);
		std::string billboardText = ParseXmlAttribute(*appearenceElement, "billboard", "");
		if (billboardText == "CameraFacingXY")
		{
			m_billboardType = CAMERA_FACING_XY;
		}
		else if (billboardText == "CameraFacingXYZ")
		{
			m_billboardType = CAMERA_FACING_XYZ;
		}
		else if (billboardText == "CameraOpposingXY")
		{
			m_billboardType = CAMERA_OPPOSING_XY;
		}
		else if (billboardText == "CameraOpposingXYZ")
		{
			m_billboardType = CAMERA_OPPOSING_XYZ;
		}

		std::string spriteSheetPath = ParseXmlAttribute(*appearenceElement, "spriteSheet", "Data/Images/Actor_Pinky_8x9.png");
		IntVec2 spriteLayout = ParseXmlAttribute(*appearenceElement, "layout", IntVec2(8,9));
		Texture* spriteTexture = g_theRenderer->CreateOrGetTextureFromFile(spriteSheetPath.c_str());
		m_spriteSheet = new SpriteSheet(*spriteTexture, spriteLayout);
		const tinyxml2::XMLElement* walkElement = appearenceElement->FirstChildElement("Walk");
		const tinyxml2::XMLElement* attackElement = appearenceElement->FirstChildElement("Attack");
		const tinyxml2::XMLElement* painElement = appearenceElement->FirstChildElement("Pain");
		const tinyxml2::XMLElement* deathElement = appearenceElement->FirstChildElement("Death");
		const tinyxml2::XMLElement* idleElement = appearenceElement->FirstChildElement("Idle");
		if (walkElement)
		{
			m_walkAnim = new SpriteAnimSet(*walkElement, m_spriteSheet);
		}
		if (attackElement)
		{
			m_attackAnim = new SpriteAnimSet(*attackElement, m_spriteSheet);
		}
		if (painElement)
		{
			m_painAnim = new SpriteAnimSet(*painElement, m_spriteSheet);
		}
		if (deathElement)
		{
			m_deathAnim = new SpriteAnimSet(*deathElement, m_spriteSheet);
		}
		if (idleElement)
		{
			m_idleAnim = new SpriteAnimSet(*idleElement, m_spriteSheet);
		}
	}
	const tinyxml2::XMLElement* gameplayElement = entityElement.FirstChildElement("Gameplay");
	if (gameplayElement)
	{
		m_damage = ParseXmlAttribute(*gameplayElement, "damage", IntRange());
	}
}

EntityDefinition::~EntityDefinition()
{
	if (m_spriteSheet != nullptr)
	{
		delete m_spriteSheet;
		m_spriteSheet = nullptr;
	}

	if (m_walkAnim != nullptr)
	{
		delete m_walkAnim;
		m_walkAnim = nullptr;
	}

	if (m_attackAnim != nullptr)
	{
		delete m_attackAnim;
		m_attackAnim = nullptr;
	}

	if (m_painAnim != nullptr)
	{
		delete m_painAnim;
		m_painAnim = nullptr;
	}

	if (m_deathAnim != nullptr)
	{
		delete m_deathAnim;
		m_deathAnim = nullptr;
	}
}

void EntityDefinition::InitializeEntityDefinitions(const tinyxml2::XMLElement& entitiesElement)
{
	const tinyxml2::XMLElement* entityelement = entitiesElement.FirstChildElement();
	while (entityelement)
	{
		std::string name = ParseXmlAttribute(*entityelement, "name", "NameError");
		if (name == "NameError")
		{
			g_theConsole->Error("Cannot find name for an entity definition");
		}
		s_definitions[name] = new EntityDefinition(*entityelement);
		entityelement = entityelement->NextSiblingElement();
	}
}

const EntityDefinition* EntityDefinition::GetDefinition(const std::string& defName)
{
	return s_definitions[defName];
}