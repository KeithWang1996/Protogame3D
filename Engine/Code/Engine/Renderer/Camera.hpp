#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Transform.hpp"

enum eCameraClearBitFlag : unsigned int
{
	CLEAR_COLOR_BIT		= (1 << 0),
	CLEAR_DEPTH_BIT		= (1 << 1),
	CLEAR_STENCIL_BIT	= (1 << 2),
};

enum eCameraMode
{
	CAMERA_MODE_ORTHOGRAPHIC,
	CAMERA_MODE_PERSPECTIVE
};
class Texture;
class RenderBuffer;
class Camera
{
	friend class RenderContext;
public:
	Camera();
	//explicit Camera( const Vec2& bottomLeft, const Vec2& topRight );
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void UpdateProjectionMatrix();
	// can use this to determine aspect ratio
	void SetOutputSize(Vec2 size);
	void SetDefaultDepthBuffer(Texture* depthbuffer) { m_depthBuffer = depthbuffer; }
	// where is the camera.  With orthographic 
	// and no ration, this is effectively where you are looking
	// - ie, the screen will be centered here.
	// - note: ignore z for now
	void SetPosition(Vec3 const& position);
	void SetCameraPitch(float pitch);
	void SetCameraYaw(float yaw);
	void RotateCameraYaw(float offset);
	void RotateCameraRoll(float offset);
	void RotateCameraPitch(float offset);
	// sets orthographic so that your screen is "height" high.  
	// can determine min and max from height, aspect ratio, and position
	void SetProjectionOrthographic(float height, float nearZ = -1.0f, float farZ = 1.0f);
	void SetProjectionPerspective(float fovDegrees, float nearZ = -1.0f, float farZ = 1.0f);
	void SetCameraMode(eCameraMode mode) { m_mode = mode; }
	// If you use the above functions, you have all the information you need for this
	// and you're a little more future proof for SD2
	Vec2 ClientToWorldPosition(Vec2 normalizedCLientPos);

	// helpers
	float GetAspectRatio() const;
	float GetCameraYaw() const { return m_transform.GetYawDegrees(); }
public:
	// used internally for now - these will eventually go away
	// but for now you can compute these from the values set above
	// to use in ClientToWorldPosition
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
public:
 
	Vec2 m_outputSize = Vec2(2.f, 1.f);
	//Vec3 m_position = Vec3(0.f, 0.f, 0.f);
	Transform m_transform;
	//Vec2 m_bottomLeft, m_topRight;
	void SetColorTarget(Texture* tex, int index = 0);
	void SetClearMode(unsigned int clearflags, Rgba8 color, float depth = 1.0f, unsigned int stencil = 0);
	Rgba8 GetClearColor() const { return m_color; };
	Mat44 GetProjection() const { return m_projection; };
	Mat44 GetView() const;
	Mat44 GetModel() const;
	Vec3 GetPosition() const { return m_transform.GetPosition(); };
	Vec3 GetForward() const;
	Vec3 GetUp() const;
	Vec3 GetLeft() const;
	void TranslateRelatively(const Vec3& translation);
	void TranslateRelativelyHorizontally(const Vec3& translation);
	void TranslateAbsolutely(const Vec3& translation);
	Mat44 GetProjection() { return m_projection; }
	void SetProjection(Mat44 projection) { m_projection = projection; }
	//void Translate2D(const Vec2& translation);
private:
	unsigned int m_clearMode = 0;
	Rgba8 m_color = Rgba8(0, 0, 0, 255);
	float m_depth = 0.f;
	Mat44 m_projection;
	float m_nearZ = -0.1f;
	float m_farZ = -100.f;
	float m_fovDegrees = 60.f;
	eCameraMode m_mode = CAMERA_MODE_PERSPECTIVE;
public:
	Texture* m_colorTarget[2];
	Texture* m_depthBuffer = nullptr;
};