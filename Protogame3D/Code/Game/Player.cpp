#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"

Player::Player(const Vec2& position)
	: Entity(position)
{
	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	m_cosmeticRadius = PLAYER_COSMETIC_RADIUS;
	m_physicsRadius = PLAYER_PHYSICS_RADIUS;
}

void Player::Update(float deltaSeconds)
{
	UpdateFromJoystick(deltaSeconds);
	Entity::Update(deltaSeconds);
}

void Player::Render() const
{
	AABB2 bounds = AABB2(Vec2(-.4f, -.4f), Vec2(.4f, .4f));
	Rgba8 tint = Rgba8(255, 255, 255, 255);
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(bounds.mins.x, bounds.mins.y, 0.f), tint, Vec2(0.f, 0.f)),//triangle 1
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(0.f, 1.f)),

		Vertex_PCU(Vec3(bounds.mins.x, bounds.maxs.y, 0.f), tint, Vec2(0.f, 1.f)),//triangle 2
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.maxs.y, 0.f), tint, Vec2(1.f, 1.f)),
		Vertex_PCU(Vec3(bounds.maxs.x, bounds.mins.y, 0.f), tint, Vec2(1.f, 0.f))
	};
	TransformVertexArray(6, vertices, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray(6, vertices);
}

void Player::UpdateFromJoystick(float deltaSeconds)
{
	if (m_controllerID < 0)
	{
		return;
	}
	const XboxController& controller = g_theInput->GetXboxController(m_controllerID);
	if (!controller.IsConnected())
	{
		return;
	}

	const AnalogJoyStick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	float leftStickOrientation = leftStick.GetAngleDegrees();
	if (leftStickMagnitude > 0.f)
	{
		m_orientationDegrees = GetTurnedToward(m_orientationDegrees, leftStickOrientation, 180.f * deltaSeconds);
	}
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, leftStickMagnitude);
}