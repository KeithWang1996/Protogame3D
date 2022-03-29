#pragma once
#define UNUSED(x) (void)(x);
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <mutex>

class BitmapFont;
class NetworkSystem;
class RandomNumberGenerator;

const Vec2 ALIGN_BOTTOM_LEFT = Vec2(0.0f, 0.0f);
const Vec2 ALIGN_CENTERED_LEFT = Vec2(0.0f, 0.5f);
const Vec2 ALIGN_TOP_LEFT = Vec2(0.0f, 1.0f);

const Vec2 ALIGN_BOTTOM_CENTERED = Vec2(0.5f, 0.0f);
const Vec2 ALIGN_CENTERED = Vec2(0.5f, 0.5f);
const Vec2 ALIGN_TOP_CENTERED = Vec2(0.5f, 1.0f);

const Vec2 ALIGN_BOTTOM_RIGHT = Vec2(1.0f, 0.0f);
const Vec2 ALIGN_CENTERED_RIGHT = Vec2(1.0f, 0.5f);
const Vec2 ALIGN_TOP_RIGHT = Vec2(1.0f, 1.0f);

constexpr float HUD_SIZE_X = 1600.f;
constexpr float HUD_SIZE_Y = 800.f;
constexpr int MATERIAL_START_INDEX = 8;
extern BitmapFont* g_theFont;
extern DevConsole* g_theConsole;
extern NamedStrings g_gameConfigBlackboard;
extern InputSystem* g_theInput;
extern EventSystem* g_theEvent;
extern NetworkSystem* g_theNetwork;
extern RandomNumberGenerator* g_theRng;

extern std::mutex g_consoleMutex;

extern std::string g_DefaultShaderCode;

extern bool g_doRenderWireFrame;