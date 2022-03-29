#pragma once
class KeyButtonState
{
public:
	void UpdateStatus(bool isNowPressed);
	bool IsPressed() const { return m_isPressed; }
	bool WasJustPressed() const;
	bool WasJustReleased() const;
	void Reset();

	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;
};