#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec4.hpp"
#include <vector>
App* g_theApp = nullptr;// Created and owned by Main_Windows.cpp
Game* g_theGame = nullptr;
Window* g_theWindow = nullptr;
RenderContext* g_theRenderer = nullptr;// Created and owned by the App
AudioSystem* g_theAudio = nullptr;
bool g_debugCameraOn = false;
bool g_physicsOn = true;

//Event Functions
void Quit()
{
	g_theWindow->m_isQuitting = true;
}

void Help()
{
	g_theConsole->PrintString(Rgba8(0, 255, 0, 255), "Here are the available commands:");
	std::vector<std::string> eventList = g_theEvent->GetEventList();
	for (int evenIndex = 0; evenIndex < eventList.size(); ++evenIndex)
	{
		g_theConsole->PrintString(Rgba8::WHITE, eventList[evenIndex]);
	}
}
void ClearDevConsole()
{
	g_theConsole->Clear();
}
void ClearDevConsoleHistory()
{
	g_theConsole->ClearHistory();
}

void DebugRenderEnableCommand(NamedStrings args)
{
	bool shouldEnable = args.GetValue("enabled", true);
	if (shouldEnable)
	{
		EnableDebugRendering();
	}
	else
	{
		DisableDebugRendering();
	}
}

void DebugAddWorldPointCommand(NamedStrings args)
{
	Vec3 pos = args.GetValue("position", Vec3(0.f, 0.f, 0.f));
	float duration = args.GetValue("duration", 0.f);
	DebugAddWorldPoint(pos, Rgba8(255, 0, 0, 255), duration, DEBUG_RENDER_USE_DEPTH);
}

void DebugAddWorldWireSphereCommand(NamedStrings args)
{
	Vec3 pos = args.GetValue("position", Vec3(0.f, 0.f, 0.f));
	float radius = args.GetValue("radius", 0.f);
	float duration = args.GetValue("duration", 0.f);
	DebugAddWorldWireSphere(pos, radius, Rgba8::WHITE, duration, DEBUG_RENDER_USE_DEPTH);
}

void DebugAddWorldWireBoundsCommand(NamedStrings args)
{
	Vec3 min = args.GetValue("min", Vec3(0.f, 0.f, 0.f));
	Vec3 max = args.GetValue("max", Vec3(0.f, 0.f, 0.f));
	float duration = args.GetValue("duration", 0.f);
	DebugAddWorldWireBounds(AABB3(min, max), Mat44::IDENTITY, Rgba8::WHITE, duration, DEBUG_RENDER_USE_DEPTH);
}

void DebugAddWorldBillBoardCommand(NamedStrings args)
{
	Vec3 pos = args.GetValue("position", Vec3(0.f, 0.f, 0.f));
	Vec2 pivot = args.GetValue("pivot", Vec2::ZERO);
	std::string text = args.GetValue("text", "test text");
	DebugAddWorldBillboardTextf(pos, pivot, Rgba8::WHITE, text.c_str());
}

void DebugAddScreenPointCommand(NamedStrings args)
{
	Vec2 pos = args.GetValue("position", Vec2::ZERO);
	float duration = args.GetValue("duration", 0.f);
	DebugAddScreenPoint(pos, 10.f, Rgba8(255, 0, 0, 255), duration);
}

void DebugAddScreenQuadCommand(NamedStrings args)
{
	Vec2 min = args.GetValue("min", Vec2::ZERO);
	Vec2 max = args.GetValue("max", Vec2::ZERO);
	float duration = args.GetValue("duration", 0.f);
	DebugAddScreenQuad(AABB2(min, max), Rgba8::WHITE, duration);
}

void DebugAddScreenTextCommand(NamedStrings args)
{
	Vec2 pos = args.GetValue("position", Vec2::ZERO);
	Vec2 pivot = args.GetValue("pivot", Vec2::ZERO);
	std::string text = args.GetValue("text", "test text");
	DebugAddScreenTextf(Vec4(0.f, 0.f, pos.x, pos.y), pivot, Rgba8::WHITE, text.c_str());
}

void SetAmbientColorCommand(NamedStrings args)
{
	Rgba8 color = args.GetValue("color", Rgba8::WHITE);
	g_theRenderer->SetAmbientColor(color);
}

void SetLightColorCommand(NamedStrings args)
{
	Rgba8 color = args.GetValue("color", Rgba8::WHITE);
	g_theGame->SetLightColor(color);
}

void SetDissolveStartColorCommand(NamedStrings args)
{
	Rgba8 color = args.GetValue("color", Rgba8::WHITE);
	g_theRenderer->SetDissolveStartColor(color);
}
void SetDissolveEndColorCommand(NamedStrings args)
{
	Rgba8 color = args.GetValue("color", Rgba8::WHITE);
	g_theRenderer->SetDissolveEndColor(color);
}