#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <queue>
#include <stack>
constexpr int MAX_XBOX_CONTROLLERS = 4;

extern const unsigned char  KEY_QUOT;
extern const unsigned char  KEY_COLON;
extern const unsigned char  KEY_QUESTION;
extern const unsigned char  KEY_COMMA;
extern const unsigned char  KEY_PERIOD;
extern const unsigned char  KEY_LEFT_BRACKET;
extern const unsigned char  KEY_RIGHT_BRACKET;
extern const unsigned char  KEY_HOME;
extern const unsigned char  KEY_END;
extern const unsigned char  KEY_SHIFT;
extern const unsigned char  KEY_SHIFT_LEFT;
extern const unsigned char  KEY_SHIFT_RIGHT;
extern const unsigned char  KEY_TAB;
extern const unsigned char  KEY_TLIDE;
extern const unsigned char  KEY_MOUSE_LEFT;
extern const unsigned char  KEY_MOUSE_RIGHT;
extern const unsigned char  KEY_MOUSE_MID;
extern const unsigned char  KEY_PLUS;
extern const unsigned char  KEY_MINUS;
extern const unsigned char	KEY_ESC;
extern const unsigned char	KEY_ENTER;
extern const unsigned char	KEY_SPACEBAR;
extern const unsigned char	KEY_BACKSPACE;
extern const unsigned char  KEY_DELETE;
extern const unsigned char	KEY_UPARROW;
extern const unsigned char	KEY_LEFTARROW;
extern const unsigned char	KEY_DOWNARROW;
extern const unsigned char	KEY_RIGHTARROW;
extern const unsigned char	KEY_F1;
extern const unsigned char	KEY_F2;
extern const unsigned char	KEY_F3;
extern const unsigned char	KEY_F4;
extern const unsigned char	KEY_F5;
extern const unsigned char	KEY_F6;
extern const unsigned char	KEY_F7;
extern const unsigned char	KEY_F8;
extern const unsigned char	KEY_F9;
extern const unsigned char	KEY_F10;
extern const unsigned char	KEY_F11;
extern const unsigned char	KEY_F12;
extern const unsigned char	KEY_LEFTCTRL;

enum eMousePositionMode
{
	MOUSE_MODE_ABSOLUTE,
	MOUSE_MODE_RELATIVE
};
struct MouseMode
{
	eMousePositionMode inputMode = MOUSE_MODE_RELATIVE;
	bool isMouseVisible = false;
	bool isMouseClipped = true;
	MouseMode(eMousePositionMode mode, bool visible, bool clipped);
};

class Window;
class InputSystem
{
	friend Window;
public:
	InputSystem();
	~InputSystem();

	void StartUp();
	void BeginFrame();
	void EndFrame();
	void ShutDown();
	void UpdateKey(unsigned char keyCode, bool isPressed);
	void UpdateMouse();
	bool IsKeyPressed(unsigned char keyCode) const;
	bool WasKeyJustPressed(unsigned char keyCode) const;
	bool WasKeyJustReleased(unsigned char keyCode) const;
	
	const XboxController& GetXboxController(int controllerID);
	const KeyButtonState& GetKey(unsigned char keyCode);

	Vec2 GetMouseNormalizedPosition() { return m_mouseNormalClientPos; }
	void AddMouseWheelScrollAmount(float deltaScrollAmount);
	float GetMouseWheelScrollAmount() { return m_scrollAmount; }
	bool WasMouseWheelScrolledUp();
	bool WasMouseWheelScrolledDown();

	void PushCharacter(char c);
	bool PopCharacter(char* out);

	void HideSystemCursor();
	void ShowSystemCursor();

	void ClipSystemCursor();
	void UnClipSystemCursor();
	void UpdateMouseMode();
	void DisableMouseMode();
	void UpdateRelativeMode();
	void PushMouseOptions(MouseMode mouseMode);
	void PopMouseOptions();
	//void SetMouseMovementMode(eMousePositionMode mode) { m_mouseMode = mode; }
	Vec2 m_relativeMovement;
private:
	XboxController m_controllers[MAX_XBOX_CONTROLLERS] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};
	KeyButtonState m_keyStates[256];
	Vec2 m_mouseNormalClientPos;
	Vec2 m_mouseNormalClientPosLastFrame = Vec2(0.5f, 0.5f);
	Vec2 m_mouseClientLastFrame;
	float m_scrollAmount = 0.f;
	float m_scrollAmountLastFrame = 0.f;
	Window* m_window = nullptr;
	std::queue<char> m_characters;
	//eMousePositionMode m_mouseMode = MOUSE_MODE_RELATIVE;
	std::stack<MouseMode> m_mouseModeStack;
};