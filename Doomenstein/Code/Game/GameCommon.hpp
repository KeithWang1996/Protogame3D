#pragma once
#include "Engine/Core/JobSystem.hpp"
class App;
class RenderContext;
class Camera;
class Game;
class Window;
class NamedStrings;
class NamedProperties;
class AudioSystem;

class JobFindLargestPrime : public Job
{
public:
	JobFindLargestPrime(int maximum);
	virtual void Execute() override;
	virtual void OnCompleteCallBack() override;

	int m_maximum;
	int m_prime = 2;
};

constexpr float CLIENT_ASPECT = 2.f;
constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 12;
constexpr int MAX_BULLETS = 20;
constexpr int MAX_BEETLES = 12;
constexpr int MAX_WASPS = 12;
constexpr int MAX_DEBRISES = 100;
constexpr int MAX_WAVES = 5;
constexpr int PLAYER_LIVES = 4;
constexpr float WORLD_SIZE_X = 20.f;
constexpr float WORLD_SIZE_Y = 40.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X * 0.5f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y * 0.5f;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_PHYSICS_RADIUS = .3f;
constexpr float PLAYER_COSMETIC_RADIUS = .5f;
constexpr float PLAYER_INITIAL_X = 3.f;
constexpr float PLAYER_INITIAL_Y = 3.f;
constexpr float PLAYER_SPEED = 10.f;
constexpr float CAMERA_INITIAL_MINX = 0.f;
constexpr float CAMERA_INITIAL_MINY = 0.f;
constexpr float CAMERA_INITIAL_MAXX = 10.f;
constexpr float CAMERA_INITIAL_MAXY = 5.f;
constexpr float	BEETLE_SPEED = 10.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.6f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.f;
constexpr float	WASP_ACCERLATION = 10.f;
constexpr float WASP_PHYSICS_RADIUS = 1.6f;
constexpr float WASP_COSMETIC_RADIUS = 2.f;
constexpr float DEBRIS_LIFESPAN = 2.f;
constexpr float DEBRIS_FADE_RATE = .5f / DEBRIS_LIFESPAN;
constexpr float MAX_WASP_SPEED = 30.f;
constexpr float MAX_SHIP_SPEED = 50.f;
constexpr float MAX_CAMERA_SHAKE_AMOUNT = 10.f;
constexpr float UI_SIZE_X = 1600.f;
constexpr float UI_SIZE_Y = 800.f;
constexpr float RESTART_COUNT_SECONDS = 3.f;
constexpr float RESPAWN_INVINCIBLE_SECONDS = 3.f;
constexpr float INVINCIBLE_FLASHING_SECONDS = 0.3f;
constexpr float DASHING_SECONDS = 0.5f;
constexpr float DASHING_COOL_DOWN = 1.0f;
constexpr float DASHING_SPEED = 80.0f;

extern App* g_theApp;
extern Window* g_theWindow;
extern Game* g_theGame;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudio;
extern JobSystem* g_theJobs;
extern bool g_debugCameraOn;
extern bool g_debugDraw;
extern bool g_physicsOn;

//Event Functions
void Quit();
void Help();
void ClearDevConsole();
void ClearDevConsoleHistory();

void DebugRenderEnableCommand(NamedStrings args);
void DebugAddWorldPointCommand(NamedStrings args);
void DebugAddWorldWireSphereCommand(NamedStrings args);
void DebugAddWorldWireBoundsCommand(NamedStrings args);
void DebugAddWorldBillBoardCommand(NamedStrings args);
void DebugAddScreenPointCommand(NamedStrings args);
void DebugAddScreenQuadCommand(NamedStrings args);
void DebugAddScreenTextCommand(NamedStrings args);
void SetAmbientColorCommand(NamedStrings args);
void SetLightColorCommand(NamedStrings args);
void SetDissolveStartColorCommand(NamedStrings args);
void SetDissolveEndColorCommand(NamedStrings args);
void WrapMap(NamedStrings args);

