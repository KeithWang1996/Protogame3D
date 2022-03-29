#pragma once
#include "Engine/Math/Vec3.hpp"
struct Mat44;
class Transform
{
public:
	Vec3 GetPosition() const { return m_position; }
	void SetPosition(Vec3 position) { m_position = position; }
	void Translate(Vec3 offset) { m_position += offset; }
	void SetRotationFromPitchRollYawDegrees(float pitchDegrees, float rollDegrees, float yawDegrees);
	void RotateYawDegrees(float offset);
	void RotatePitchDegrees(float offset);
	void RotateRollDegrees(float offset);
	void SetYawDegrees(float yawDegrees);
	void SetPitchDegrees(float pitchDegrees);
	float GetPitchDegrees() const { return m_pitchDegrees; }
	float GetRollDegrees() const { return m_rollDegrees; }
	float GetYawDegrees() const { return m_yawDegrees; }
	Mat44 GetAsMatrix() const;
	Mat44 GetAsViewMatrix() const;
	Mat44 GetRotationAsMatrix() const;
private:
	Vec3 m_position;
	float m_pitchDegrees = 0.f;
	float m_rollDegrees = 0.f;
	float m_yawDegrees = 0.f;
	bool m_pitchIncrementFlag = false;
};