#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Platform/Window.hpp"
//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove all OpenGL references out of Main_Win32.cpp once you have a RenderContext
// Both of the following lines should be relocated to the top of Engine/Renderer/RenderContext.cpp
//
//#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
//#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
//#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to Game/GameCommon.hpp or elsewhere
// 
//constexpr float CLIENT_ASPECT = 2.f; // We are requesting a 1:1 aspect (square) window area


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
bool g_isReadyToQuit = false;							// ...becomes App::m_isQuitting

const char* APP_NAME = "SD3-A1: Protogame 3D";	// ...becomes ??? (Change this per project!)

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//

//------------------------------------------------------------------------------------------------
// Given an existing OS Window, create a Rendering Context (RC) for, say, OpenGL to draw to it.
//
// void CreateRenderContext()
// {
// 	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
// 	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
// 	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
// 	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
// 	pixelFormatDescriptor.nVersion = 1;
// 	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
// 	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
// 	pixelFormatDescriptor.cColorBits = 24;
// 	pixelFormatDescriptor.cDepthBits = 24;
// 	pixelFormatDescriptor.cAccumBits = 0;
// 	pixelFormatDescriptor.cStencilBits = 8;
// 
// 	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
// 	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
// 	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
// 	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );
// 
// 	// #SD1ToDo: move all OpenGL functions (including those below) to RenderContext.cpp (only!)
// 	//glEnable( GL_BLEND );
// 	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
// }
//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );
	//TheApp_Startup( applicationInstanceHandle, commandLineString );		// #SD1ToDo: ...becomes:  g_theApp = new App();  AND  g_theApp->Startup();

	g_theApp = new App();
	
	g_theWindow = new Window();
	g_theWindow->Open(APP_NAME, CLIENT_ASPECT, .9f);

	g_theApp->Startup();
	// Program main loop; keep running frames until it's time to quit
	//while( !g_isReadyToQuit )			// #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	while(!g_theApp->IsQuitting())
	{
		g_theWindow->BeginFrame();
		g_theApp->RunFrame();
	}
	g_theApp->Shutdown();
	//TheApp_Shutdown();				// #SD1ToDo: ...becomes:  g_theApp->Shutdown();  AND  delete g_theApp;  AND  g_theApp = nullptr;
	delete g_theApp;
	g_theApp = nullptr;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;
	return 0;
}


