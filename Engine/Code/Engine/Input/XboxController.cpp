#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility

XboxController::XboxController(int controllerID)
	:m_controllerID(controllerID)
{}

XboxController::~XboxController()
{}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);
	if (errorStatus == ERROR_SUCCESS)
	{
		m_isConnected = true;
		UpdateTrigger(m_leftTriggerValue, xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerValue, xboxControllerState.Gamepad.bRightTrigger);
		UpdateJoystick(m_leftJoystick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightJoystick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
		for (int buttonStateIndex = 0; buttonStateIndex < NUM_XBOX_BUTTONS; ++buttonStateIndex)
		{
			UpdateButton(static_cast<XboxButtonID>(buttonStateIndex), xboxControllerState.Gamepad.wButtons, static_cast<unsigned short>(m_buttonMasks[buttonStateIndex]));
		}
	}
	else
	{
		m_isConnected = false;
		Reset();
	}
}

void XboxController::Reset()
{
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	m_leftTriggerValue = 0.f;
	m_rightTriggerValue = 0.f;
	for (int buttonStateIndex = 0; buttonStateIndex < NUM_XBOX_BUTTONS; ++buttonStateIndex)
	{
		m_buttonStates[buttonStateIndex].Reset();
	}
}

void XboxController::UpdateTrigger(float& triggerValue, unsigned char rawValue)
{
	triggerValue = RangeMapFloat(0.f, 255.f, 0.f, 1.f, static_cast<float>(rawValue));
}

void XboxController::UpdateJoystick(AnalogJoyStick& joystick, short rawX, short rawY)
{
	float normalizedX = RangeMapFloat(static_cast<float>(m_rawJoystickMinimum), static_cast<float>(m_rawJoystickMaximum), -1.f, 1.f, static_cast<float>(rawX));
	float normalizedY = RangeMapFloat(static_cast<float>(m_rawJoystickMinimum), static_cast<float>(m_rawJoystickMaximum), -1.f, 1.f, static_cast<float>(rawY));
	joystick.UpdatePosition(normalizedX, normalizedY);
}

void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	if ((buttonFlag & buttonFlags) == buttonFlag)
	{
		m_buttonStates[buttonID].UpdateStatus(true);
	}
	else {
		m_buttonStates[buttonID].UpdateStatus(false);
	}
}

void XboxController::EndFrame()
{
	for (int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++buttonIndex)
	{
		m_buttonStates[buttonIndex].m_wasPressedLastFrame = m_buttonStates[buttonIndex].m_isPressed;
	}
}