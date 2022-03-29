#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_Lit.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Player.hpp"
#include "Game/World.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Audio/AudioSystem.hpp"
Game::Game()
{
	m_player = nullptr;
	m_world = nullptr;
	m_camera = new Camera();
	m_UICamera = new Camera();
	m_debugCamera = new Camera();
	//m_camera->SetOrthoView(Vec2(0.f, 0.f), Vec2(80.f, 40.f));
	m_camera->SetPosition(Vec3(0.f, 0.f, 0.f));
	m_camera->SetProjectionPerspective(60.f, -0.01f, -100.f);
	m_camera->SetDefaultDepthBuffer(g_theRenderer->GetDefaultDepthBuffer());
	m_camera->SetClearMode(CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT, Rgba8(0, 0, 0, 255));
	Vec2 bottomLeft = m_camera->GetOrthoBottomLeft();
	Vec2 topRight = m_camera->GetOrthoTopRight();
	m_debugCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(80.f, 40.f));
	m_inverseShader = g_theRenderer->GetOrCreateShader("Data/Shaders/InverseColor.hlsl");

	//test passed
	m_cubeMesh = new GPUMesh(g_theRenderer);
	std::vector<unsigned int> indices;
	std::vector<Vertex_Lit> vertices;
	AddAABB3ToIndexedVertexArray(vertices, indices, AABB3(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f), Rgba8::WHITE);
	m_cubeMesh->UpdateIndices(indices);
	m_cubeMesh->UpdateVertices(vertices);
	m_cubeTransform.SetPosition(Vec3(2.5f, 0.5f, 0.5f));
	m_cubeTransform1.SetPosition(Vec3(0.5f, 2.5f, 0.5f));
	m_cubeTransform2.SetPosition(Vec3(2.5f, 2.5f, 0.5f));

	//
	m_basisMesh = new GPUMesh(g_theRenderer);
	std::vector<unsigned int> basisIndices;
	std::vector<Vertex_PCU> basisVertices;
	AddBasisToIndexVertexArray(basisVertices, basisIndices, Mat44::IDENTITY);
	m_basisMesh->UpdateIndices(basisIndices);
	m_basisMesh->UpdateVertices(basisVertices);
	
	m_compassMesh = new GPUMesh(g_theRenderer);
	std::vector<unsigned int> compassIndices;
	std::vector<Vertex_PCU> compassVertices;
	AddBasisToIndexVertexArray(compassVertices, compassIndices, Mat44::IDENTITY, 0.01f);
	m_compassMesh->UpdateIndices(compassIndices);
	m_compassMesh->UpdateVertices(compassVertices);
	
	m_clock = new Clock();
	DebugAddWorldPoint(Vec3(20.f, 10.f, 0.5f), Rgba8::WHITE, 100.f);

	//Set up Shaders
	m_shaders[0] = g_theRenderer->GetOrCreateShader("Data/Shaders/WorldOpaque.hlsl");
	m_shaders[1] = g_theRenderer->m_defaultShader;
	m_shaders[2] = g_theRenderer->GetOrCreateShader("Data/Shaders/normal.hlsl");
	m_shaders[3] = g_theRenderer->GetOrCreateShader("Data/Shaders/tangent.hlsl");
	m_shaders[4] = g_theRenderer->GetOrCreateShader("Data/Shaders/bitangent.hlsl");
	m_shaders[5] = g_theRenderer->GetOrCreateShader("Data/Shaders/surfacenormal.hlsl");
	m_triplanarShader = g_theRenderer->GetOrCreateShader("Data/Shaders/triplanar.hlsl");
	m_projectionShader = g_theRenderer->GetOrCreateShader("Data/Shaders/projection.hlsl");
	//Set up attenuations
	m_attenuations[0] = Vec3(0.f, 1.f, 0.f);
	m_attenuations[1] = Vec3(0.f, 0.f, 1.f);
	m_attenuations[2] = Vec3(1.f, 0.f, 0.f);

	//Set up textures for test
	m_tileDiffuse = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/tile_diffuse.png");
	m_tileNormal = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/tile_normal.png");
	m_tileDiffuse1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/brick_color.png");
	m_tileNormal1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/brick_normal.png");
	m_tileDiffuse2 = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_colour.png");
	m_tileNormal2 = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_normal.png");
	m_dissolveMaterial = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/noise.png");
	m_heightMap = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/brick_height.png");
	m_meshDiffuse = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/diffuse.jpg");

	//Obj loading test
	std::vector<Vertex_Lit> modelVerts;
	mesh_import_options_t options;
	options.generate_normals = true;
	options.generate_tangents = true;
	LoadOBJToVertexArray(modelVerts, "Data/Meshes/ship.obj", options);
	m_modelMesh = new GPUMesh(g_theRenderer);
	m_modelMesh->UpdateVertices(modelVerts);
	
	//audio test
	m_testSound = g_theAudio->CreateOrGetSound("Data/Audios/TestSound.mp3");

	//game config and xml test
	tinyxml2::XMLDocument doc;
	doc.LoadFile("GameConfig.xml");
	tinyxml2::XMLElement* root;
	root = doc.RootElement();
	tinyxml2::XMLElement* child;
	child = root->FirstChildElement();
	m_UIIsOn = ParseXmlAttribute(*child, "UIIsOn", false);
	m_UIColor = ParseXmlAttribute(*child, "UIColor", Rgba8::WHITE);
}

Game::~Game()
{
	delete m_camera;
	delete m_UICamera;
	delete m_player;
	delete m_world;
	delete m_debugCamera;
	delete m_cubeMesh;
	delete m_compassMesh;
	delete m_cubeSphereMesh;
	delete m_basisMesh;
	delete m_modelMesh;
	delete m_clock;
	m_camera = nullptr;
	m_UICamera = nullptr;
	m_player = nullptr;
	m_world = nullptr;
	m_debugCamera = nullptr;
	m_cubeMesh = nullptr;
	m_compassMesh = nullptr;
	m_cubeSphereMesh = nullptr;
	m_basisMesh = nullptr;
	m_modelMesh = nullptr;
	m_clock = nullptr;
}

void Game::ResetGame()
{
	m_playerLives = PLAYER_LIVES;
	m_debugOn = false;
	m_gameIsOn = false;
	m_gameIsOver = false;
	m_stage = 0;//0 for attract, 1 for game, 2 for victory;
	m_wave = 0;
	m_shakeCameraIntensity = 0.f;
	m_restartCountSeconds = RESTART_COUNT_SECONDS;
}

void Game::HandleCollision() {
}


void Game::Update()
{
	if (m_clock->IsPaused())
	{
		return;
	}
	float deltaSeconds = (float)m_clock->GetLastDeltaSeconds();
	if (!g_theConsole->IsOpen())
	{
		UpdateFromKeyboard(deltaSeconds);
	}
	if (m_timeEachFrame < 0.016f)
	{
		m_timeEachFrame += deltaSeconds;
	}
	else
	{
		if (m_clearColorChangeState)
		{
			m_clearColorRed--;
			m_clearColorBlue++;
			if (m_clearColorRed == 0)
			{
				m_clearColorChangeState = false;
			}
		}
		else
		{
			m_clearColorRed++;
			m_clearColorBlue--;
			if (m_clearColorBlue == 0)
			{
				m_clearColorChangeState = true;
			}
		}
		m_timeEachFrame = 0.f;
	}
	if (m_lightMoveStatus[m_lightIndex] == LIGHT_FOLLOW_CAMERA)
	{
		m_lights[m_lightIndex].world_position = m_camera->GetPosition();
		m_lights[m_lightIndex].direction = m_camera->GetForward();
	}
	m_animationAngleDegrees += (float)m_clock->GetLastDeltaSeconds() * 20.f;
	if (m_animationAngleDegrees > 360.f)
	{
		m_animationAngleDegrees -= 360.f;
	}
	for (int i = 0; i < 8; ++i)
	{
		if (m_lightMoveStatus[i] == LIGHT_ANIMATE)
		{
			m_lights[i].world_position = SphericalToCartesian(m_animationAngleDegrees, 0.f, m_animationRadius) + Vec3(0.f, 0.f, -10.f);
		}
	}
	
}

void Game::Render() const
{
	Texture* backbuffer = g_theRenderer->GetDefaultColorTarget();
	Texture* frameTarget = g_theRenderer->AcquireRenderTargetMatching(backbuffer);
	Texture* bloomTarget = g_theRenderer->AcquireRenderTargetMatching(backbuffer);
	m_camera->SetColorTarget(frameTarget);
	m_camera->SetColorTarget(bloomTarget, 1);
	BeginCamera();
	g_theRenderer->BindShader(m_shaders[m_shaderIndex]);
	//Handle light
	g_theRenderer->SetAmbientIntensity(m_ambientIntensity);
	
	for (int i = 0; i < 8; ++i)
	{
		g_theRenderer->EnableLight(m_lights[i], i);
	}
	
	g_theRenderer->SetSpecularFactor(m_specularFactor);
	g_theRenderer->SetSpecularPower(m_specularPower);
	
	g_theRenderer->FinalizeLight();
	
	g_theRenderer->FinalizeMaterial();


	//Draw cubes
	g_theRenderer->BindTexture(g_theRenderer->m_defaultTexture);

	g_theRenderer->SetModelMatrix(m_cubeTransform.GetAsMatrix());
 	g_theRenderer->DrawMesh(m_cubeMesh);

	g_theRenderer->SetModelMatrix(m_cubeTransform1.GetAsMatrix());
	g_theRenderer->DrawMesh(m_cubeMesh);

	g_theRenderer->SetModelMatrix(m_cubeTransform2.GetAsMatrix());
	g_theRenderer->DrawMesh(m_cubeMesh);

	g_theRenderer->SetDepthTest(COMPARE_FUNC_ALWAYS, true);
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->SetModelMatrix(Mat44::IDENTITY);
	g_theRenderer->DrawMesh(m_basisMesh);

	Vec3 compassPosition = m_camera->GetForward() * 0.1f + m_camera->GetPosition();
	g_theRenderer->SetModelMatrix(Mat44::CreateTranslation3D(compassPosition));
	g_theRenderer->DrawMesh(m_compassMesh);
	EndCamera();

	Texture* blurredBloom = g_theRenderer->AcquireRenderTargetMatching(bloomTarget);
	Shader* blurShader = g_theRenderer->GetOrCreateShader("Data/Shaders/blur.hlsl");
	g_theRenderer->BeginEffect(blurredBloom, bloomTarget, blurShader);
	g_theRenderer->EndEffect();

	g_theRenderer->ReleaseRenderTarget(frameTarget);
	g_theRenderer->ReleaseRenderTarget(bloomTarget);
	g_theRenderer->ReleaseRenderTarget(blurredBloom);
	//Make sure not keep generating.
	GUARANTEE_OR_DIE((g_theRenderer->GetTotalRenderTargetPoolSize()) < 5, "Keep generate render targets");

	Shader* shader = g_theRenderer->GetOrCreateShader("Data/Shaders/effect.hlsl");
	g_theRenderer->BeginEffect(backbuffer, frameTarget, shader);
	if (m_isBloomOn)
	{
		g_theRenderer->BindNormalTexture(blurredBloom);
	}
	else
	{
		g_theRenderer->BindNormalTexture(g_theRenderer->m_emptyTexture);
	}
	g_theRenderer->SetGrayScaleForce(m_grayScaleFactor);
	g_theRenderer->SetTintForce(m_tintFactor);
	g_theRenderer->SetGrayScaleTint(Vec4(1.f, 0.f, 0.f, 1.f));
	g_theRenderer->EndEffect();
	DebugRenderWorldToCamera(m_camera);
	if (m_UIIsOn)
	{
		DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -30.f), Vec2(0.f, 0.5f), 30.f, m_UIColor, 0.f, "Camera Yaw: %f, Pitch: %f, Roll: %f", m_camera->m_transform.GetYawDegrees(), m_camera->m_transform.GetPitchDegrees(), m_camera->m_transform.GetRollDegrees());
		DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -60.f), Vec2(0.f, 0.5f), 30.f, m_UIColor, 0.f, "Camera X: %f, Y: %f, Z: %f", m_camera->GetPosition().x, m_camera->GetPosition().y, m_camera->GetPosition().z);
		DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -90.f), Vec2(0.f, 0.5f), 30.f, m_UIColor, 0.f, "IBasis: forward, +X when identity: (%f, %f, %f)", m_camera->GetForward().x, m_camera->GetForward().y, m_camera->GetForward().z);
		DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -120.f), Vec2(0.f, 0.5f), 30.f, m_UIColor, 0.f, "JBasis: left, +Y when identity: (%f, %f, %f)", m_camera->GetLeft().x, m_camera->GetLeft().y, m_camera->GetLeft().z);
		DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -150.f), Vec2(0.f, 0.5f), 30.f, m_UIColor, 0.f, "KBasis: up, +Z when identity: (%f, %f, %f)", m_camera->GetUp().x, m_camera->GetUp().y, m_camera->GetUp().z);
	}
	
	//Debug render
	/*
	std::string shaderMode;
	switch (m_shaderIndex)
	{
	case 0: 
		shaderMode = "Lit";
		break;
	case 1:
		shaderMode = "Color Only";
		break;
	case 2:
		shaderMode = "Normal";
		break;
	case 3:
		shaderMode = "Tangent";
		break;
	case 4:
		shaderMode = "Bitangent";
		break;
	case 5:
		shaderMode = "Surface Normal";
		break;
	default:
		break;
	}
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -30.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[<, >] - Shader Mode: %s", shaderMode.c_str());
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -60.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[9, 0] - Ambient Intensity: %f", m_ambientIntensity);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -90.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[T] - Attenuation: (%f, %f, %f)", 
		m_lights[m_lightIndex].attenuation.x, m_lights[m_lightIndex].attenuation.y, m_lights[m_lightIndex].attenuation.z);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -120.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[-, +] - Light Intensity: %f", m_lights[m_lightIndex].intensity);

	for (int i = 0; i < 8; ++i)
	{
		DebugAddWorldPoint(m_lights[i].world_position, m_lightColor[i]);
	}
	//DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -150.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[F5] - Light To Origin");
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -180.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[F6] - Light To Camera");
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -210.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[F7] - Light Follow Camera");
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -240.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[F8] - Light Animation");

	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -270.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[{, }] - Specular Factor: %f", m_specularFactor);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -300.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[:, '] - Specular Power: %f", m_specularPower);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -330.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[Q, E] - Current Light: %i", m_lightIndex);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -360.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[R] - Loop Light Type");
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -390.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[F] - Toggle Fog");
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -420.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[V ,B] - Dissolve Depth: %f", m_dissolveDepth);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -450.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[N, M] - Dissolve Range: %f", m_dissolveRange);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -480.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[1 ,2] - Parallex Depth: %f", m_parallexDepth);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -510.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[3, 4] - Parallex Step: %i", m_parallexSteps);

	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -540.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "Total RT Create: %i", g_theRenderer->GetTotalRenderTargetPoolSize());
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -570.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "Current RT in pool: %i", g_theRenderer->GetTexturePoolFreeCount());

	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -600.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[5, 6] - Gray Scale Factor: %f", m_grayScaleFactor);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -630.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[7, 8] - Gray Scale Tint Factor: %f", m_tintFactor);
	DebugAddScreenTextf(Vec4(0.f, 1.f, 10.f, -670.f), Vec2(0.f, 0.5f), 30.f, Rgba8::WHITE, 0.f, "[Z] - Bloom On/Off");
	*/
}

void Game::RenderDebug() const
{
	m_world->RenderDebug();
}

void Game::BeginCamera() const
{
	g_theRenderer->BeginCamera(*m_camera);
}

void Game::EndCamera() const
{
	g_theRenderer->EndCamera(*m_camera);
}

void Game::TestFunction()
{
	g_theConsole->PrintString(Rgba8::WHITE, "Specular Factor is: " + std::to_string(m_specularFactor));
}

void Game::BeginDebugCamera() const
{
	g_theRenderer->BeginCamera(*m_debugCamera);
}
void Game::EndDebugCamera() const
{
	g_theRenderer->EndCamera(*m_debugCamera);
}

void Game::BeginUICamera() const
{
	g_theRenderer->BeginCamera(*m_UICamera);
}

void Game::EndUICamera() const
{
	g_theRenderer->EndCamera(*m_UICamera);
}

void Game::UpdateCamera(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	Vec2 playerPosition = m_world->GetPlayerPosition();
	AABB2 cameraAABB = AABB2(Vec2(CAMERA_INITIAL_MINX, CAMERA_INITIAL_MINY), Vec2(CAMERA_INITIAL_MAXX, CAMERA_INITIAL_MAXY));
	cameraAABB.SetCenter(playerPosition);
	cameraAABB.FitWithinBounds(AABB2(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y)));
	TranslateCamera2D(cameraAABB.mins.x, cameraAABB.mins.y);
}

void Game::UpdateUICamera()
{
	TranslateUICamera2D(0.f, 0.f);
}

void Game::TranslateCamera2D(float xOffset, float yOffset)
{
	//m_camera->SetOrthoView(Vec2(WORLD_CENTER_X - 10.f + xOffset, WORLD_CENTER_Y - 5.f + yOffset), Vec2(WORLD_CENTER_X + 10.f + xOffset, WORLD_CENTER_Y + 5.f + yOffset));

	m_camera->SetOrthoView(Vec2(CAMERA_INITIAL_MINX + xOffset, CAMERA_INITIAL_MINY + yOffset), Vec2(CAMERA_INITIAL_MAXX + xOffset, CAMERA_INITIAL_MAXY + yOffset));
}

void Game::TranslateUICamera2D(float xOffset, float yOffset)
{
	m_UICamera->SetOrthoView(Vec2(0.f + xOffset, 0.f + yOffset), Vec2(UI_SIZE_X + xOffset, UI_SIZE_Y + yOffset));
}

void Game::ShakeCamera()
{
	m_shakeCameraIntensity = 1.f;
}

void Game::SetLightColor(Rgba8 color)
{
	m_lightColor[m_lightIndex] = color;
	m_lights[m_lightIndex].color = color.GetAsVec4().XYZ();
}

Vec3 Game::LoopAttenuation(const Vec3& currAtt)
{
	if (currAtt == Vec3(1.f, 0.f, 0.f))
	{
		return Vec3(0.f, 1.f, 0.f);
	}
	if (currAtt == Vec3(0.f, 1.f, 0.f))
	{
		return Vec3(0.f, 0.f, 1.f);
	}
	else
	{
		return Vec3(1.f, 0.f, 0.f);
	}
}

void Game::LoopLightTypeAtIndex(int index)
{
	switch (m_lightTypes[index])
	{
	case LIGHT_TYPE_POINT:
		m_lights[index].innerAngle = m_innerAngleCos;
		m_lights[index].outerAngle = m_outerAngleCos;
		m_lightTypes[index] = LIGHT_TYPE_CONE;
		break;
	case LIGHT_TYPE_CONE:
		m_lights[index].innerAngle = -1.f;
		m_lights[index].outerAngle = -1.f;
		m_lights[index].directionFactor = 1.f;
		m_lightTypes[index] = LIGHT_TYPE_DIRECTIONAL;
		break;
	case LIGHT_TYPE_DIRECTIONAL:
		m_lights[index].directionFactor = 0.f;
		m_lightTypes[index] = LIGHT_TYPE_POINT;
		break;
	default:
		break;
	}
}



void Game::RenderAttract() const
{
// 	g_theRenderer->DrawSegment(Vec2(0.f, 0.f), Vec2(200.f, 100.f), 0.1f, Rgba8(255, 255, 255, 255));
// 	g_theRenderer->DrawSegment(Vec2(0.f, 100.f), Vec2(200.f, 0.f), 0.1f, Rgba8(255, 255, 255, 255));
// 	Texture* tankBaseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
// // 	g_theRenderer->BindTexture(nullptr);
// // 	g_theRenderer->DrawAABB2(Vec2(WORLD_CENTER_X - 10.f, WORLD_CENTER_Y - 10.f), Vec2(WORLD_CENTER_X + 10.f, WORLD_CENTER_Y + 10.f), Rgba8(255, 255, 255, 255));
// // 	g_theRenderer->BindTexture(nullptr);
}

void Game::RenderVictory() const
{
}

void Game::RenderGame() const
{
	if (m_debugOn)
	{
		RenderDebug();
	}
}

void Game::UpdateFromJoystick(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}


void Game::UpdateFromKeyboard(float deltaSeconds)
{
	if (g_theInput->WasKeyJustPressed(KEY_F4))
	{
		g_debugCameraOn = !g_debugCameraOn;
	}

	if (g_theInput->WasKeyJustPressed(KEY_F1))
	{
		//m_debugOn = !m_debugOn;
		//PlaySound(SoundID soundID, bool isLooped = false, float volume = 1.f, float balance = 0.0f, float speed = 1.0f, bool isPaused = false);
		float volume = g_theRng->RollRandomFloatInRange(0.5f, 1.0f);
		float balance = g_theRng->RollRandomFloatInRange(-1.0f, 1.0f);
		float speed = g_theRng->RollRandomFloatInRange(0.5f, 2.0f);
		g_theAudio->PlaySound(m_testSound, false, volume, balance, speed);
	}

	if (g_theInput->WasKeyJustPressed(KEY_F3))
	{
		g_physicsOn = !g_physicsOn;
	}

	Vec3 movement;
	Vec3 movementAbsolutely;
	float speed = 2.0f;
	if (g_theInput->IsKeyPressed('W'))
	{
		movement += speed * deltaSeconds * Vec3(1.f, 0.f, 0.f);
	}
	if (g_theInput->IsKeyPressed('S'))
	{
		movement += speed * deltaSeconds * Vec3(-1.f, 0.f, 0.f);
	}
	if (g_theInput->IsKeyPressed('A'))
	{
		movement += speed * deltaSeconds * Vec3(0.f, 1.f, 0.f);
	}
	if (g_theInput->IsKeyPressed('D'))
	{
		movement += speed * deltaSeconds * Vec3(0.f, -1.f, 0.f);
	}
	if (g_theInput->IsKeyPressed('E'))
	{
		movementAbsolutely += speed * deltaSeconds * Vec3(0.f, 0.f, 1.f);
	}
	if (g_theInput->IsKeyPressed('Q'))
	{
		movementAbsolutely += speed * deltaSeconds * Vec3(0.f, 0.f, -1.f);
	}
	if (g_theInput->IsKeyPressed(KEY_SHIFT))
	{
		m_camera->TranslateRelativelyHorizontally(movement * 2.f);
		m_camera->TranslateAbsolutely(movementAbsolutely * 2.f);
	}
	else
	{
		m_camera->TranslateRelativelyHorizontally(movement);
		m_camera->TranslateAbsolutely(movementAbsolutely);
	}

	//if (g_theInput->m_relativeMovement.x < -0.02f)
	{
		m_camera->RotateCameraYaw(-g_theInput->m_relativeMovement.x * deltaSeconds * 2000.f);
	}
	//if (g_theInput->m_relativeMovement.y < -0.02f)
	{
		m_camera->RotateCameraPitch(-g_theInput->m_relativeMovement.y * deltaSeconds * 2000.f);
	}

	if (g_theInput->WasKeyJustPressed('X'))
	{
		g_doRenderWireFrame = !g_doRenderWireFrame;
	}

// 	if (g_theInput->WasKeyJustPressed(KEY_PERIOD))
// 	{
// 		m_shaderIndex++;
// 		m_shaderIndex = m_shaderIndex % 6;
// 	}
// 
// 	if (g_theInput->WasKeyJustPressed(KEY_COMMA))
// 	{
// 		m_shaderIndex--;
// 		if (m_shaderIndex < 0)
// 		{
// 			m_shaderIndex += 6;
// 		}
// 	}
	if (g_theInput->IsKeyPressed('9'))
	{
		m_ambientIntensity -= (float)m_clock->GetLastDeltaSeconds();
		if (m_ambientIntensity < 0.f)
		{
			m_ambientIntensity = 0.f;
		}
	}

	if (g_theInput->IsKeyPressed('0'))
	{
		m_ambientIntensity += (float)m_clock->GetLastDeltaSeconds();
		if (m_ambientIntensity > 1.f)
		{
			m_ambientIntensity = 1.f;
		}
	}

	if (g_theInput->WasKeyJustPressed('T'))
	{
		m_lights[m_lightIndex].attenuation = LoopAttenuation(m_lights[m_lightIndex].attenuation);
	}

	if (g_theInput->IsKeyPressed(KEY_MINUS))
	{
		m_lights[m_lightIndex].intensity -= (float)m_clock->GetLastDeltaSeconds();
		if (m_lights[m_lightIndex].intensity < 0.f)
		{
			m_lights[m_lightIndex].intensity = 0.f;
		}
	}

	if (g_theInput->IsKeyPressed(KEY_PLUS))
	{
		m_lights[m_lightIndex].intensity += (float)m_clock->GetLastDeltaSeconds();
	}

	if (g_theInput->WasKeyJustPressed(KEY_F5))
	{
		m_lights[m_lightIndex].world_position = Vec3(0.f, 0.f, 0.f);
		m_lightMoveStatus[m_lightIndex] = LIGHT_STATIC;
	}

	if (g_theInput->WasKeyJustPressed(KEY_F6))
	{
		m_lights[m_lightIndex].world_position = m_camera->GetPosition();
		m_lightMoveStatus[m_lightIndex] = LIGHT_STATIC;
	}

	if (g_theInput->WasKeyJustPressed(KEY_F7))
	{
		m_lightMoveStatus[m_lightIndex] = LIGHT_FOLLOW_CAMERA;
	}

	if (g_theInput->WasKeyJustPressed(KEY_F8))
	{
		m_lightMoveStatus[m_lightIndex] = LIGHT_ANIMATE;
	}

	if (g_theInput->IsKeyPressed(KEY_LEFT_BRACKET))
	{
		m_specularFactor -= (float)m_clock->GetLastDeltaSeconds();
		if (m_specularFactor < 0.f)
		{
			m_specularFactor = 0.f;
		}
	}
	if (g_theInput->IsKeyPressed(KEY_RIGHT_BRACKET))
	{
		m_specularFactor += (float)m_clock->GetLastDeltaSeconds();
		if (m_specularFactor > 1.f)
		{
			m_specularFactor = 1.f;
		}
	}

	if (g_theInput->IsKeyPressed(KEY_COLON))
	{
		m_specularPower -= (float)m_clock->GetLastDeltaSeconds() * 10.f;
		if (m_specularPower < 0.f)
		{
			m_specularPower = 0.f;
		}
	}
	if (g_theInput->IsKeyPressed(KEY_QUOT))
	{
		m_specularPower += (float)m_clock->GetLastDeltaSeconds() * 10.f;
	}
// 	if (g_theInput->WasKeyJustPressed('Q'))
// 	{
// 		m_lightIndex--;
// 		if (m_lightIndex < 0)
// 		{
// 			m_lightIndex += 8;
// 		}
// 	}
// 	if (g_theInput->WasKeyJustPressed('E'))
// 	{
// 		m_lightIndex++;
// 		if (m_lightIndex > 7)
// 		{
// 			m_lightIndex -= 8;
// 		}
// 	}
	if (g_theInput->WasKeyJustPressed('R'))
	{
		LoopLightTypeAtIndex(m_lightIndex);
	}
	if (g_theInput->WasKeyJustPressed('F'))
	{
		if (m_isFogOn)
		{
			g_theRenderer->DisableFog();
			m_isFogOn = false;
		}
		else
		{
			g_theRenderer->EnableFog(0.f, 30.f, Rgba8(0, 0, 0, 255), Rgba8(0, 0, 0, 255));
			m_isFogOn = true;
		}
	}
	if (g_theInput->IsKeyPressed('V'))
	{
		m_dissolveDepth -= (float)m_clock->GetLastDeltaSeconds();
		if (m_dissolveDepth < 0.f)
		{
			m_dissolveDepth = 0.f;
		}
		g_theRenderer->SetDissolveDepth(m_dissolveDepth);
	}
	if (g_theInput->IsKeyPressed('B'))
	{
		m_dissolveDepth += (float)m_clock->GetLastDeltaSeconds();
		if (m_dissolveDepth > 1.f)
		{
			m_dissolveDepth = 1.f;
		}
		g_theRenderer->SetDissolveDepth(m_dissolveDepth);
	}
	if (g_theInput->IsKeyPressed('N'))
	{
		m_dissolveRange -= (float)m_clock->GetLastDeltaSeconds();
		if (m_dissolveRange < 0.f)
		{
			m_dissolveRange = 0.f;
		}
		g_theRenderer->SetDissolveRange(m_dissolveRange);
	}
	if (g_theInput->IsKeyPressed('M'))
	{
		m_dissolveRange += (float)m_clock->GetLastDeltaSeconds();
		if (m_dissolveRange > 0.5f)
		{
			m_dissolveRange = 0.5f;
		}
		g_theRenderer->SetDissolveRange(m_dissolveRange);
	}

	if (g_theInput->IsKeyPressed('1'))
	{
		m_parallexDepth -= (float)m_clock->GetLastDeltaSeconds();
		if (m_parallexDepth < 0.f)
		{
			m_parallexDepth = 0.f;
		}
		g_theRenderer->SetParallexDepth(m_parallexDepth);
	}
	if (g_theInput->IsKeyPressed('2'))
	{
		m_parallexDepth += (float)m_clock->GetLastDeltaSeconds();
		if (m_parallexDepth > .25f)
		{
			m_parallexDepth = .25f;
		}
		g_theRenderer->SetParallexDepth(m_parallexDepth);
	}
	if (g_theInput->WasKeyJustPressed('3'))
	{
		m_parallexSteps -= 1;
		if (m_parallexSteps < 1)
		{
			m_parallexSteps = 1;
		}
		g_theRenderer->SetParallexStep(m_parallexSteps);
	}
	if (g_theInput->WasKeyJustPressed('4'))
	{
		m_parallexSteps += 1;
		if (m_parallexSteps > 32)
		{
			m_parallexSteps = 32;
		}
		g_theRenderer->SetParallexStep(m_parallexSteps);
	}

	if (g_theInput->WasKeyJustPressed('Z'))
	{
		m_isBloomOn = !m_isBloomOn;
	}

	if (g_theInput->IsKeyPressed('5'))
	{
		m_grayScaleFactor -= (float)m_clock->GetLastDeltaSeconds();
		if (m_grayScaleFactor < 0.f)
		{
			m_grayScaleFactor = 0.f;
		}
	}
	if (g_theInput->IsKeyPressed('6'))
	{
		m_grayScaleFactor += (float)m_clock->GetLastDeltaSeconds();
		if (m_grayScaleFactor > 1.f)
		{
			m_grayScaleFactor = 1.f;
		}
	}

	if (g_theInput->IsKeyPressed('7'))
	{
		m_tintFactor -= (float)m_clock->GetLastDeltaSeconds();
		if (m_tintFactor < 0.f)
		{
			m_tintFactor = 0.f;
		}
	}
	if (g_theInput->IsKeyPressed('8'))
	{
		m_tintFactor += (float)m_clock->GetLastDeltaSeconds();
		if (m_tintFactor > 1.f)
		{
			m_tintFactor = 1.f;
		}
	}
}