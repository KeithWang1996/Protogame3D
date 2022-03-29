#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp" 
#include "Engine/Renderer/Texture.hpp" 
#include "Engine/Math/Vec3.hpp"

Camera::Camera()
{
	m_colorTarget[0] = nullptr;
	m_colorTarget[1] = nullptr;
}

void Camera::SetOutputSize(Vec2 size)
{
	m_outputSize = size;
	UpdateProjectionMatrix();
}

void Camera::SetProjectionOrthographic(float height, float nearZ, float farZ)
{
	m_farZ = farZ;
	m_nearZ = nearZ;
	m_outputSize = Vec2(height * GetAspectRatio(), height);
	UpdateProjectionMatrix();
}

void Camera::SetProjectionPerspective(float fovDegrees, float nearZ, float farZ)
{
	//m_projection = Mat44::CreatePerspectiveProjection(fovDegrees, nearZ, farZ, GetAspectRatio());
	m_farZ = farZ;
	m_nearZ = nearZ;
	m_fovDegrees = fovDegrees;
	UpdateProjectionMatrix();
}

float Camera::GetAspectRatio() const
{
	return m_outputSize.x / m_outputSize.y;
}

Vec2 Camera::ClientToWorldPosition(Vec2 normalizedClientPos)
{
	Vec2 orthoMin = GetOrthoBottomLeft();
	Vec2 orthoMax = GetOrthoTopRight();
	float worldPosX = RangeMapFloat(0.f, 1.f, orthoMin.x, orthoMax.x, normalizedClientPos.x);
	float worldPosY = RangeMapFloat(0.f, 1.f, orthoMin.y, orthoMax.y, normalizedClientPos.y);
	// These TODOs are notes on things that will change in this function
	// as MP2 & SD2 advance;
	// TODO - take into account render target
	//        clientPos being the pixel location on the texture

	// TODO - use projection matrix to compute this

	// TODO - Support ndc-depth paramater for 3D-coordinates, needed for ray casts.

	// TODO - take into account viewport
	return Vec2(worldPosX, worldPosY);
}
void Camera::SetPosition(const Vec3& position)
{
	m_transform.SetPosition(position);
	UpdateProjectionMatrix();
}

void Camera::TranslateRelatively(const Vec3& translation)
{
	Mat44 rotMat = m_transform.GetRotationAsMatrix();
	Vec3 trans = translation;
	trans = rotMat.TransformVector3D(trans);
	m_transform.Translate(trans);
	UpdateProjectionMatrix();
}

void Camera::TranslateRelativelyHorizontally(const Vec3& translation)
{
	Mat44 rotMat = m_transform.GetRotationAsMatrix();
	Vec3 trans = translation;
	trans = rotMat.TransformVector3D(trans);
	trans.z = 0.f;
	m_transform.Translate(trans);
	UpdateProjectionMatrix();
}

void Camera::TranslateAbsolutely(const Vec3& translation)
{
	m_transform.Translate(translation);
	UpdateProjectionMatrix();
}

void Camera::SetCameraYaw(float yaw)
{
	m_transform.SetYawDegrees(yaw);
}

void Camera::SetCameraPitch(float pitch)
{
	m_transform.SetPitchDegrees(pitch);
}

void Camera::RotateCameraYaw(float offset)
{
	m_transform.RotateYawDegrees(offset);
}

void Camera::RotateCameraRoll(float offset)
{
	m_transform.RotateRollDegrees(offset);
}

void Camera::RotateCameraPitch(float offset)
{
	m_transform.RotatePitchDegrees(Clamp(offset, -89.9f, 89.9f));
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	Vec2 position = (bottomLeft + topRight) * 0.5f;
	m_transform.SetPosition(Vec3(position, 0.f));
	m_outputSize = Vec2(topRight.x - bottomLeft.x, topRight.y - bottomLeft.y);
	UpdateProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	if (m_mode == CAMERA_MODE_ORTHOGRAPHIC)
	{
		m_projection = Mat44::CreateOrthographicProjection(Vec3(0.f, 0.f, m_nearZ), Vec3(m_outputSize, m_farZ));
	}
	else
	{
		m_projection = Mat44::CreatePerspectiveProjection(m_fovDegrees, m_nearZ, m_farZ, GetAspectRatio());
	}
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return Vec2(m_transform.GetPosition().x - (m_outputSize.x * 0.5f), m_transform.GetPosition().y - (m_outputSize.y * 0.5f));
}

Vec2 Camera::GetOrthoTopRight() const
{
	return Vec2(m_transform.GetPosition().x + (m_outputSize.x * 0.5f), m_transform.GetPosition().y + (m_outputSize.y * 0.5f));
}

void Camera::SetClearMode(unsigned int clearflags, Rgba8 color, float depth, unsigned int stencil)
{
	UNUSED(stencil);
	m_clearMode = clearflags;
	m_color = color;
	m_depth = depth;
}

Mat44 Camera::GetView() const
{
	Mat44 viewMat;
	if (m_mode == CAMERA_MODE_ORTHOGRAPHIC)
	{
		viewMat = Mat44::CreateTranslation3D(-Vec3(GetOrthoBottomLeft(), 0.f));
	}
	else
	{
 		viewMat.TransformBy(Mat44::CONVERSION_NZ_NX_Y);
 		viewMat.TransformBy(m_transform.GetAsViewMatrix());
	}
	return viewMat;
}

Mat44 Camera::GetModel() const
{
	return m_transform.GetAsMatrix();
}

Vec3 Camera::GetForward() const
{
	Vec3 forward = Vec3(1.f, 0.f, 0.f);
	Mat44 model = GetModel();
	model.SetTranslation3D(Vec3(0.f, 0.f, 0.f));
	return model.TransformVector3D(forward);
}

Vec3 Camera::GetUp() const
{
	Vec3 up = Vec3(0.f, 0.f, 1.f);
	Mat44 model = GetModel();
	model.SetTranslation3D(Vec3(0.f, 0.f, 0.f));
	return model.TransformVector3D(up);
}

Vec3 Camera::GetLeft() const
{
	Vec3 forward = Vec3(0.f, 1.f, 0.f);
	Mat44 model = GetModel();
	model.SetTranslation3D(Vec3(0.f, 0.f, 0.f));
	return model.TransformVector3D(forward);
}

void Camera::SetColorTarget(Texture* tex, int index)
{
	m_colorTarget[index] = tex;
}