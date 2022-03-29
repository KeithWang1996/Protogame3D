#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "AnalogJoystick.hpp"
enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_LEFT,
	XBOX_BUTTON_ID_DOWN,

	NUM_XBOX_BUTTONS
};

class XboxController
{
	friend class InputSystem;
public:
	explicit XboxController(int controllerID);
	~XboxController();


	bool					IsConnected() const			{ return m_isConnected; }
	int						GetControllerID() const		{ return m_controllerID; }
	const AnalogJoyStick&	GetLeftJoystick() const		{ return m_leftJoystick; }
	const AnalogJoyStick&	GetRightJoystick() const	{ return m_rightJoystick; }
	float					GetLeftTrigger() const		{ return m_leftTriggerValue; }
	float					GetRightTrigger() const		{ return m_rightTriggerValue; }
	const KeyButtonState&	GetButtonState( XboxButtonID buttonID) const { return m_buttonStates[buttonID]; }
private:
	void Update();
	void Reset();
	void UpdateTrigger(float& triggerValue, unsigned char rawValue);
	void UpdateJoystick(AnalogJoyStick& joystick, short rawX, short rawY);
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);
	void EndFrame();
	const short	m_rawJoystickMaximum = 32767;
	const short m_rawJoystickMinimum = -32768;
public:
	const int		m_controllerID = -1;
	bool			m_isConnected = false;
	KeyButtonState  m_buttonStates[NUM_XBOX_BUTTONS];
	/*
	#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

	*/
	int			m_buttonMasks[NUM_XBOX_BUTTONS] = 
	{
					0x1000,		//A
					0x2000,		//B
					0x4000,		//X
					0x8000,		//Y
					0x0020,		//Back
					0x0010,		//Start
					0x0100,		//LT
					0x0200,		//RT
					0x0040,		//LS
					0x0080,		//RS
					0x0008,		//Right
					0x0001,		//Up
					0x0004,		//Left
					0x0002,		//Down
	};
	AnalogJoyStick  m_leftJoystick = AnalogJoyStick(0.3f, 0.95f);
	AnalogJoyStick  m_rightJoystick = AnalogJoyStick(0.3f, 0.95f);
	float			m_leftTriggerValue = 0.f;
	float			m_rightTriggerValue = 0.f;
};