#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec4.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/DebugRender.hpp"

const unsigned char  KEY_QUOT = 0xDE;
const unsigned char  KEY_COLON = 0xBA;
const unsigned char  KEY_QUESTION = 0xBF;
const unsigned char  KEY_COMMA = 0xBC;
const unsigned char  KEY_PERIOD = 0xBE;
const unsigned char  KEY_LEFT_BRACKET = 0xDB;
const unsigned char  KEY_RIGHT_BRACKET = 0xDD;
const unsigned char  KEY_HOME = 0x24;
const unsigned char  KEY_END = 0x23;
const unsigned char  KEY_SHIFT = 0x10;
const unsigned char  KEY_SHIFT_LEFT = 0xA0;
const unsigned char  KEY_SHIFT_RIGHT = 0xA1;
const unsigned char  KEY_TAB = 0x09;
const unsigned char  KEY_TLIDE = 0xC0;
const unsigned char  KEY_MOUSE_LEFT = 0x01;
const unsigned char  KEY_MOUSE_RIGHT = 0x02;
const unsigned char  KEY_MOUSE_MID = 0x04;
const unsigned char  KEY_PLUS = 0xBB;
const unsigned char  KEY_MINUS = 0xBD;
const unsigned char	 KEY_ESC = 0x1B;
const unsigned char	 KEY_ENTER = 0x0D;
const unsigned char	 KEY_SPACEBAR = 0x20;
const unsigned char	 KEY_BACKSPACE = 0x08;
const unsigned char	 KEY_DELETE = 0x2E;
const unsigned char	 KEY_UPARROW = 0x26;
const unsigned char	 KEY_LEFTARROW = 0x25;
const unsigned char	 KEY_DOWNARROW = 0x28;
const unsigned char	 KEY_RIGHTARROW = 0x27;
const unsigned char	 KEY_F1 = 0x70;
const unsigned char	 KEY_F2 = 0x71;
const unsigned char	 KEY_F3 = 0x72;
const unsigned char	 KEY_F4 = 0x73;
const unsigned char	 KEY_F5 = 0x74;
const unsigned char	 KEY_F6 = 0x75;
const unsigned char	 KEY_F7 = 0x76;
const unsigned char	 KEY_F8 = 0x77;
const unsigned char	 KEY_F9 = 0x78;
const unsigned char	 KEY_F10 = 0x79;
const unsigned char	 KEY_F11 = 0x7A;
const unsigned char	 KEY_F12 = 0x7B;
const unsigned char	 KEY_LEFTCTRL = 0x11;

InputSystem::InputSystem()
{}

InputSystem::~InputSystem()
{}

void InputSystem::StartUp()
{

}
void InputSystem::BeginFrame()
{
	for (int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].Update();
	}
}
void InputSystem::EndFrame()
{
	//update was pressed last frame
	for (int keyIndex = 0; keyIndex < 256; ++keyIndex)
	{
		m_keyStates[keyIndex].m_wasPressedLastFrame = m_keyStates[keyIndex].m_isPressed;
	}
	for (int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].EndFrame();
	}
	m_scrollAmountLastFrame = m_scrollAmount;
	//clear the queue
	while (!m_characters.empty())
	{
		m_characters.pop();
	}
}
void InputSystem::ShutDown()
{

}

const XboxController& InputSystem::GetXboxController(int controllerID)
{
	return m_controllers[controllerID];
}

void InputSystem::AddMouseWheelScrollAmount(float deltaScrollAmount)
{
	m_scrollAmount += deltaScrollAmount;
}

bool InputSystem::WasMouseWheelScrolledUp()
{
	if (m_scrollAmount > m_scrollAmountLastFrame)
	{
		return true;
	}
	return false;
}

bool InputSystem::WasMouseWheelScrolledDown()
{
	if (m_scrollAmount < m_scrollAmountLastFrame)
	{
		return true;
	}
	return false;
}

void InputSystem::UpdateKey(unsigned char keyCode, bool isPressed)
{
	m_keyStates[keyCode].UpdateStatus(isPressed);
}

void InputSystem::UpdateMouse()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient((HWND)m_window->m_hwnd, &mousePos);
	Vec2 mouseClientPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

	RECT clientRect;
	GetClientRect((HWND)m_window->m_hwnd, &clientRect);
	AABB2 clientBounds(static_cast<float>(clientRect.left), static_cast<float>(clientRect.top), static_cast<float>(clientRect.right), static_cast<float>(clientRect.bottom));
	m_mouseNormalClientPos = clientBounds.GetUVForPoint(mouseClientPos);
	m_mouseNormalClientPos.y = 1.f - m_mouseNormalClientPos.y;
	if (m_window->m_isActivated)
	{
		if (m_mouseModeStack.top().inputMode == MOUSE_MODE_RELATIVE)
		{
			UpdateRelativeMode();
		}
	}
	if (m_window->wasJustActivated())
	{
		UpdateMouseMode();
	}
	if (m_window->wasJustDeactivated())
	{
		DisableMouseMode();
	}
}

const KeyButtonState& InputSystem::GetKey(unsigned char keyCode)
{
	return m_keyStates[keyCode];
}

bool InputSystem::IsKeyPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].IsPressed();
}
bool InputSystem::WasKeyJustPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].WasJustPressed();
}
bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	return m_keyStates[keyCode].WasJustReleased();
}

void InputSystem::PushCharacter(char c)
{
	m_characters.push(c);
}

bool InputSystem::PopCharacter(char* out)
{
	if (!m_characters.empty()) {
		*out = m_characters.front();
		m_characters.pop();
		return true;
	}
	else {
		return false;
	}
}

void InputSystem::HideSystemCursor()
{
	while (ShowCursor(false) > -1){}
}
void InputSystem::ShowSystemCursor()
{
	while (ShowCursor(true) < 0) {}
}

void InputSystem::ClipSystemCursor()
{
	RECT clientRect;
	GetWindowRect((HWND)m_window->m_hwnd, &clientRect);
	ClipCursor(&clientRect);
}

void InputSystem::UnClipSystemCursor()
{
	ClipCursor(nullptr);
}

void InputSystem::UpdateRelativeMode()
{
	//Vec2 positionThisFrame = m_mouseNormalClientPos;
	m_relativeMovement = m_mouseNormalClientPos - m_mouseNormalClientPosLastFrame;
	
	//DebugAddScreenTextf(Vec4(0.5f, 0.5f, 100, 0), Vec2(0.5, 0.5), 30.f, Rgba8(255, 0, 0, 255), 0.0001f, "%f", m_mouseNormalClientPos.y);
		// move back to center
	IntVec2 windowCenter = m_window->GetWindowCenter(); // GetClientRect() to get rectangle, find center of that
	SetCursorPos(windowCenter.x, windowCenter.y);
	
 	POINT mousePos;
 	GetCursorPos(&mousePos);
 	ScreenToClient((HWND)m_window->m_hwnd, &mousePos);
 	Vec2 mouseClientPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
 
 	RECT clientRect;
 	GetClientRect((HWND)m_window->m_hwnd, &clientRect);
 	AABB2 clientBounds(static_cast<float>(clientRect.left), static_cast<float>(clientRect.top), static_cast<float>(clientRect.right), static_cast<float>(clientRect.bottom));
	m_mouseNormalClientPosLastFrame = clientBounds.GetUVForPoint(mouseClientPos);
	m_mouseNormalClientPosLastFrame.y = 1.f - m_mouseNormalClientPosLastFrame.y;
}

MouseMode::MouseMode(eMousePositionMode mode, bool visible, bool clipped)
	: inputMode(mode)
	, isMouseVisible(visible)
	, isMouseClipped(clipped)
{}

void InputSystem::PushMouseOptions(MouseMode mouseMode)
{
	m_mouseModeStack.push(mouseMode);
	UpdateMouseMode();
}

void InputSystem::PopMouseOptions()
{
	m_mouseModeStack.pop();
	UpdateMouseMode();
}

void InputSystem::UpdateMouseMode()
{
	if (m_mouseModeStack.top().isMouseVisible)
	{
		ShowSystemCursor();
	}
	else
	{
		HideSystemCursor();
	}

	if (m_mouseModeStack.top().isMouseClipped)
	{
		ClipSystemCursor();
	}
	else
	{
		UnClipSystemCursor();
	}
}

void InputSystem::DisableMouseMode()
{
	ShowSystemCursor();
	UnClipSystemCursor();
}