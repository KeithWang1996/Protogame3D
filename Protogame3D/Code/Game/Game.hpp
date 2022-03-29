#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
struct Vec2;
class Player;
class World;
class Shader;
class GPUMesh;
class Clock;
class Texture;
class NamedProperties;
enum eLightStatus
{
	LIGHT_STATIC,
	LIGHT_FOLLOW_CAMERA,
	LIGHT_ANIMATE
};
enum eLightType
{
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_CONE,
	LIGHT_TYPE_DIRECTIONAL
};
class Game {
public:
	Game();
	~Game();
	//void Initialize();
	void HandleCollision();
	//void HandleShipAsteroidCollision();
	void Update();
	void Render() const;//should be const
	void RenderDebug() const;
	void RenderAttract() const;
	void RenderVictory() const;
	void RenderGame() const;
	void ResetGame();
	void BeginCamera() const;
	void EndCamera() const;
	void TestFunction();
	void BeginDebugCamera() const;
	void EndDebugCamera() const;
	void BeginUICamera() const;
	void EndUICamera() const;
	void UpdateCamera(float deltaSeconds);
	void UpdateUICamera();
	void UpdateFromJoystick(float deltaSeconds);
	void UpdateFromKeyboard(float deltaSeconds);
	void TranslateCamera2D(float xOffset, float yOffset);
	void TranslateUICamera2D(float xOffset, float yOffset);
	void ShakeCamera();
	Camera* getCamera() const { return m_camera; }
	void SetLightColor(Rgba8 color);
public:
	Clock* m_clock = nullptr;
private:
	Vec3 LoopAttenuation(const Vec3& currAtt);
	void LoopLightTypeAtIndex(int index);
private:
	bool m_debugOn = false;
	bool m_gameIsOn = false;
	bool m_gameIsOver = false;
	int m_stage = 0;//0 for attract, 1 for game, 2 for victory;
	int m_wave = 0;
	int m_playerLives = PLAYER_LIVES;
	int m_controllerID = -1;
	Camera* m_camera;
	Camera* m_UICamera;
	Camera* m_debugCamera;
	float m_shakeCameraIntensity = 0.f;
	float m_restartCountSeconds = RESTART_COUNT_SECONDS;
	Player* m_player = nullptr;
	World* m_world = nullptr;
	//for color switching
	bool m_clearColorChangeState = true;
	unsigned char m_clearColorRed = 255;
	unsigned char m_clearColorBlue = 0;
	float m_timeEachFrame = 0.f;
	Shader* m_inverseShader;
	Shader* m_triplanarShader;
	Shader* m_projectionShader;
	Shader* m_shaders[6];
	int m_shaderIndex = 0;
	Vec3 m_attenuations[3];
	int m_attIndex = 0;
	float m_ambientIntensity = 0.5f;
	eLightStatus m_lightMoveStatus[8];
	eLightType m_lightTypes[8];
	float m_animationAngleDegrees = 0.f;
	float m_animationRadius = 10.f;
	float m_specularFactor = 1.0f;
	float m_specularPower = 20.f;
	int m_lightIndex = 0;
	float m_innerAngleCos = 0.98f;
	float m_outerAngleCos = 0.96f;
	light_t m_lights[8];
	Rgba8 m_lightColor[8];
	bool m_isFogOn = true;
	bool m_isBloomOn = true;
	float m_dissolveDepth = 0.f;
	float m_dissolveRange = 0.f;
	float m_parallexDepth = 0.f;

	float m_grayScaleFactor = 0.f;
	float m_tintFactor = 0.f;
	int m_parallexSteps = 10;

	GPUMesh* m_cubeMesh;
	GPUMesh* m_compassMesh;
	GPUMesh* m_cubeSphereMesh;
	GPUMesh* m_basisMesh;
	GPUMesh* m_modelMesh;
	Transform m_cubeTransform;
	Transform m_cubeTransform2;
	Transform m_cubeTransform1;
	Transform m_compassTransform;

	Texture* m_tileDiffuse;
	Texture* m_tileNormal;
	Texture* m_tileDiffuse1;
	Texture* m_tileNormal1;
	Texture* m_tileDiffuse2;
	Texture* m_tileNormal2;
	Texture* m_dissolveMaterial;
	Texture* m_heightMap;
	Texture* m_meshDiffuse;

	SoundID m_testSound;

	Rgba8 m_UIColor = Rgba8::WHITE;
	bool m_UIIsOn = false;
};