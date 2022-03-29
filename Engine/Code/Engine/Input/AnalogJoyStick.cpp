#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"
AnalogJoyStick::AnalogJoyStick(float innerDeadZoneFraction, float outerDeadZoneFraction)
	:m_innerDeadZoneFraction(innerDeadZoneFraction)
	,m_outerDeadZoneFraction(outerDeadZoneFraction)
{}

void AnalogJoyStick::Reset()
{
	m_rawPosition = Vec2(0.f, 0.f);
	m_correctedPosition = Vec2(0.f, 0.f);;
	m_correctedDegrees = 0.f;
	m_correctedMagnitude = 0.f;
}

void AnalogJoyStick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);
	float rawMagnitude = m_rawPosition.GetLength();
	m_correctedMagnitude = RangeMapFloat(m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f, rawMagnitude);
	if (rawMagnitude < m_innerDeadZoneFraction)
	{
		m_correctedMagnitude = 0.f;
	}
	else {
		m_correctedDegrees = m_rawPosition.GetAngleDegrees();
	}
	if (rawMagnitude > m_outerDeadZoneFraction)
	{
		m_correctedMagnitude = 1.f;
	}
	while (m_correctedDegrees < 0.f)
	{
		m_correctedDegrees += 360.f;
	}
	while (m_correctedDegrees > 360.f)
	{
		m_correctedDegrees -= 360.f;
	}

	m_correctedPosition = Vec2::MakeFromPolarDegrees(m_correctedDegrees, m_correctedMagnitude);
}