#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
void Transform::SetRotationFromPitchRollYawDegrees(float pitchDegrees, float rollDegrees, float yawDegrees)
{
	RotatePitchDegrees(pitchDegrees - m_pitchDegrees);
	RotateRollDegrees(rollDegrees - m_rollDegrees);
	RotateYawDegrees(yawDegrees - m_yawDegrees);
}

void Transform::RotateYawDegrees(float offset)
{
	m_yawDegrees = (float)fmod(m_yawDegrees + offset + 180.f, 360.f) - 180.f;
}

void Transform::RotatePitchDegrees(float offset)
{
// 	if (m_pitchIncrementFlag)
// 	{
// 		offset = -offset;
// 	}
// 
// 	m_pitchDegrees = m_pitchDegrees + offset;
// 	while (m_pitchDegrees > 90.f || m_pitchDegrees < -90.f)
// 	{
// 		if (m_pitchDegrees > 90.f)
// 		{
// 			m_pitchDegrees = 90.f - (m_pitchDegrees - 90.f);
// 		}
// 		if (m_pitchDegrees < -90.f)
// 		{
// 			m_pitchDegrees = -90.f - (m_pitchDegrees + 90.f);
// 		}
// 		RotateRollDegrees(180.f);
// 		RotateYawDegrees(180.f);
// 		m_pitchIncrementFlag = !m_pitchIncrementFlag;
// 	}
	m_pitchDegrees = (float)fmod(m_pitchDegrees + offset + 180.f, 360.f) - 180.f;
}

void Transform::RotateRollDegrees(float offset)
{
	m_rollDegrees = (float)fmod(m_rollDegrees + offset + 180.f, 360.f) - 180.f;
}

void Transform::SetYawDegrees(float yawDegrees)
{
	m_yawDegrees = (float)fmod(yawDegrees + 180.f, 360.f) - 180.f;
}

void Transform::SetPitchDegrees(float pitchDegrees)
{
	m_pitchDegrees = Clamp(pitchDegrees, -90.f, 90.f);
}

Mat44 Transform::GetAsMatrix() const
{
	Mat44 translateMat = Mat44::CreateTranslation3D(m_position);
	Mat44 rotationMat = GetRotationAsMatrix();
	//rotationMat.SetTranslation3D(m_position);
	//translateMat.TransformBy(rotationMat);
	rotationMat.TransformBy(translateMat);
	return rotationMat;

// 	//Mat44 translateMat = Mat44::CreateTranslation3D(m_position);
// 	Mat44 rotationMat = GetRotationAsMatrix();
// 	rotationMat.SetTranslation3D(m_position);
// 	//translateMat.TransformBy(rotationMat);
// 	return rotationMat;
}

Mat44 Transform::GetAsViewMatrix() const
{
	Mat44 rotationMat = Mat44::IDENTITY;
	rotationMat.RotationXDegrees(-GetRollDegrees());
	rotationMat.RotationYDegrees(-GetPitchDegrees());
	rotationMat.RotationZDegrees(-GetYawDegrees());
	rotationMat.TransformBy(Mat44::CreateTranslation3D(-GetPosition()));
	return rotationMat;
}

Mat44 Transform::GetRotationAsMatrix() const
{
	Mat44 rotationMat = Mat44::IDENTITY;
	rotationMat.RotationZDegrees(GetYawDegrees());
	rotationMat.RotationYDegrees(GetPitchDegrees());
	rotationMat.RotationXDegrees(GetRollDegrees());
	return rotationMat;
// 	Mat44 rotationMat = Mat44::CreateYRotationDegrees(m_yawDegrees);
// 	rotationMat.RotationZDegrees(m_rollDegrees);
// 	rotationMat.RotationXDegrees(m_pitchDegrees);
// 	return rotationMat;
}