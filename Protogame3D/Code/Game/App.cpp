#include "Game/App.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
//extern to ensure there is a object
//App* g_theApp = nullptr;// Created and owned by Main_Windows.cpp
//RenderContext* g_theRenderer = nullptr;// Created and owned by the App

void App::Startup()
{
	g_theConsole = new DevConsole();
	g_theConsole->StartUp();
	m_HUDCamera = new Camera();
	m_HUDCamera->SetCameraMode(CAMERA_MODE_ORTHOGRAPHIC);
	m_HUDCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(HUD_SIZE_X, HUD_SIZE_Y));
	m_HUDCamera->SetProjectionOrthographic(800.f, 1.f, -100.f);
	g_theRenderer = new RenderContext();
	g_theRenderer->Startup(g_theWindow);
	g_theRenderer->Setup(nullptr);
	g_theInput = new InputSystem();
	g_theInput->StartUp();
	g_theAudio = new AudioSystem();
	g_theRng = new RandomNumberGenerator();
	//camera = new Camera();
	g_theFont = g_theRenderer->CreateOrGetBitmapFromFIle("Data/Fonts/SquirrelFixedFont");
	DebugRenderSystemStartup(g_theRenderer);
	g_theGame = new Game();
	g_theWindow->SetInputSystem(g_theInput);
	g_theInput->PushMouseOptions(MouseMode(MOUSE_MODE_RELATIVE, false, true));
	g_theEvent = new EventSystem();
	g_theConsole->PrintString(Rgba8(255, 0, 0, 255), "Game Started");
	g_theConsole->m_window = g_theWindow;
	Clock::SystemStartup();
	//subscribe event
	g_theEvent->SubscribeToEvent("help", Help);
	g_theEvent->SubscribeToEvent("quit", Quit);
	g_theEvent->SubscribeToEvent("clear", ClearDevConsole);
	g_theEvent->SubscribeToEvent("clear history", ClearDevConsoleHistory);

	g_theEvent->SubscribeToEvent("debug_render", DebugRenderEnableCommand);

	g_theEvent->SubscribeToEvent("debug_add_world_point", DebugAddWorldPointCommand);
	g_theEvent->SubscribeToEvent("debug_add_world_wire_sphere", DebugAddWorldWireSphereCommand);
	g_theEvent->SubscribeToEvent("debug_add_world_wire_bounds", DebugAddWorldWireBoundsCommand);
	g_theEvent->SubscribeToEvent("debug_add_world_billboard_text", DebugAddWorldBillBoardCommand);
	g_theEvent->SubscribeToEvent("debug_add_screen_point", DebugAddScreenPointCommand);
	g_theEvent->SubscribeToEvent("debug_add_screen_quad", DebugAddScreenQuadCommand);
	g_theEvent->SubscribeToEvent("debug_add_screen_text", DebugAddScreenTextCommand);
	g_theEvent->SubscribeToEvent("light_set_ambient_color", SetAmbientColorCommand);
	g_theEvent->SubscribeToEvent("light_set_color", SetLightColorCommand);
	g_theEvent->SubscribeToEvent("dissolve_set_start_color", SetDissolveStartColorCommand);
	g_theEvent->SubscribeToEvent("dissolve_set_end_color", SetDissolveEndColorCommand);
}

void App::Shutdown()
{
	g_theRenderer->Shutdown();
	DebugRenderSystemShutdown();
	Clock::SystemShutdown();
}

App::App()
{}

App::~App()
{
	delete g_theEvent;
	delete g_theRenderer;
	//delete camera;
	delete g_theGame;
	delete g_theInput;
	delete g_theConsole;
	delete g_theAudio;
	delete g_theRng;
	g_theEvent = nullptr;
	g_theRenderer = nullptr;
	//camera = nullptr;
	g_theGame = nullptr;
	g_theInput = nullptr;
	g_theConsole = nullptr;
	g_theAudio = nullptr;
	g_theRng = nullptr;
}

void App::BeginFrame()
{
	g_theAudio->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	Clock::BeginFrame();
	DebugRenderBeginFrame();
}

void App::EndFrame()
{
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	DebugRenderEndFrame();
}

void App::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timeThisFrameStarted = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float>(timeThisFrameStarted - timeLastFrameStarted);
	if (deltaSeconds > 0.1f)
	{
		deltaSeconds = 0.1f;
	}
	timeLastFrameStarted = timeThisFrameStarted;
	BeginFrame();			// for all engine systems(Not game)
	Update();	// the game ONLY
	Render();				//the game ONLY
	EndFrame();				// for all engine systems(Not game)
}

void App::Update()
{
	g_theGame->Update();
	g_theConsole->Update();
	UpdateFromKeyboard();
	g_theInput->UpdateMouse();
	DebugRenderUpdate();
}

bool App::HandleQuitRequested()
{
	//m_isQuitting = true;
	g_theWindow->m_isQuitting = true;
	return true;
}

bool App::IsQuitting() const
{
	return g_theWindow->m_isQuitting;
}

void App::Render() const
{
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (10,10) top-right
	// #SD1ToDo: This will be replaced by a call to g_renderer->BeginView( m_worldView ); or similar
	// 	glLoadIdentity();
	// 	glOrtho( 0.f, 200.f, 0.f, 100.f, 0.f, 1.f );
	//g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	//camera->SetOrthoView(Vec2(0.f, 0.f), Vec2(200, 100));
	//g_theRenderer->BeginCamera(*camera);
	// Clear all screen (back buffer) pixels to medium-blue
	// #SD1ToDo: This will become g_renderer->ClearColor( Rgba8( 0, 0, 127, 255 ) );
	// 	glClearColor( 0.f, 0.f, 0.0f, 1.f ); // Note; glClearColor takes colors as floats in [0,1], not bytes in [0,255]
	// 	glClear( GL_COLOR_BUFFER_BIT ); // ALWAYS clear the screen at the top of each frame's Render()!
	// Draw some triangles (provide 3 vertexes each)
	//RenderShip();
	g_theGame->Render();
	DebugRenderScreenTo(g_theRenderer->GetDefaultColorTarget());
	g_theConsole->Render(*g_theRenderer, *m_HUDCamera);
	
	//g_theRenderer->EndCamera(*camera);
	// #SD1ToDo: Move all OpenGL code into RenderContext.cpp (only); call g_renderer->DrawVertexArray() instead
	// 	glBegin( GL_TRIANGLES );
	// 	{
	// 		// First triangle (3 vertexes, each preceded by a color)
	// 		glColor4ub( 255, 0, 0, 255 );
	// 		glVertex3f( 1.f, 1.f, 0.f );
	// 
	// 		glColor4ub( 0, 255, 0, 255 );
	// 		glVertex3f( 8.f, 2.f, 0.f );
	// 
	// 		glColor4ub( 255, 255, 255, 255 );
	// 		glVertex3f( 3.f, 8.f, 0.f );
	// 
	// 	}
	// 	glEnd();
	// 	DrawFilledAABB2( 2.f, 2.f, 8.f, 4.f, 255, 0, 0 ,255);
}

void App::ResetGame()
{
	delete g_theGame;
	g_theGame = new Game();
}

void App::UpdateFromKeyboard()
{
	if(g_theInput->WasKeyJustPressed(KEY_ESC))
	{
		if (g_theConsole->IsOpen())
		{
			g_theConsole->SetIsOpen(false);
			g_theGame->m_clock->Resume();
		}
		else
		{
			g_theWindow->m_isQuitting = true;
		}
	}
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->IsKeyPressed('T'))
	{
		m_isSlowMo = true;
	}
	else
	{
		m_isSlowMo = false;
	}

	if (g_theInput->IsKeyPressed('Y'))
	{
		m_isFastMo = true;
	}
	else
	{
		m_isFastMo = false;
	}

	if (g_theInput->WasKeyJustPressed(KEY_F8))
	{
		//ResetGame();
	}
	if (g_theInput->WasKeyJustPressed(KEY_TLIDE))
	{
		(g_theConsole)->SetIsOpen(!g_theConsole->IsOpen());
		if (g_theGame->m_clock->IsPaused())
		{
			g_theGame->m_clock->Resume();
		}
		else
		{
			g_theGame->m_clock->Pause();
		}
	}
}