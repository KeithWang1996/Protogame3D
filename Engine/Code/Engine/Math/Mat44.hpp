#pragma once
struct Vec2;
struct Vec3;
struct Vec4;

struct Mat44
{
public:
	float Ix = 1.f;
	float Iy = 0.f;
	float Iz = 0.f;
	float Iw = 0.f;
	float Jx = 0.f;
	float Jy = 1.f;
	float Jz = 0.f;
	float Jw = 0.f;
	float Kx = 0.f;
	float Ky = 0.f;
	float Kz = 1.f;
	float Kw = 0.f;
	float Tx = 0.f;
	float Ty = 0.f;
	float Tz = 0.f;
	float Tw = 1.f;

	const static Mat44 IDENTITY;
	const static Mat44 CONVERSION_NZ_NX_Y;
public:
	Mat44() = default;
	explicit Mat44(float* sixteenValuesBasisMajor);
	explicit Mat44(Vec2& iBasis2D, Vec2& jBasis2D, Vec2& translation2D);
	explicit Mat44(Vec3& iBasis3D, Vec3& jBasis3D, Vec3& kBasis3D, Vec3& translation3D);
	explicit Mat44(Vec4& iBasisHomogeneous, Vec4& jBasisHomogeneous, Vec4& kBasisHomogeneous, Vec4& translationHomogeneous);

	const Vec2 TransformVector2D(const Vec2& vectorQuantity) const;
	const Vec3 TransformVector3D(const Vec3& vectorQuantity) const;
	const Vec2 TransformPosition2D(const Vec2& position) const;
	const Vec3 TransformPosition3D(const Vec3& position) const;
	const Vec4 TransformHomogeneousPosition3D(const Vec4& homogeneousPoint) const;

	const float*	GetAsFloatArray() const			{ return &Ix; }
	float*			GetAsFloatArray()				{ return &Ix; }
	const Vec2		GetIBasis2D() const;
	const Vec2		GetJBasis2D() const;
	const Vec2		GetTranslation2D() const;

	const Vec3		GetIBasis3D() const;
	const Vec3		GetJBasis3D() const;
	const Vec3		GetKBasis3D() const;
	const Vec3		GetTranslation3D() const;

	const Vec4		GetIBasis4D() const;
	const Vec4		GetJBasis4D() const;
	const Vec4		GetKBasis4D() const;
	const Vec4		GetTranslation4D() const;

	void SetTranslation2D(const Vec2& translation2D);
	void SetTranslation3D(const Vec3& translation3D);
	void SetBasisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D);
	void SetBasisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	void SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D);
	void SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D);
	void SetBasisVectors4D(const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D);

	void RotationXDegrees(float degreeAboutX);
	void RotationYDegrees(float degreeAboutY);
	void RotationZDegrees(float degreeAboutZ);

	void Translate2D(const Vec2& translationXY);
	void Translate3D(const Vec3& translation3D);
	void ScaleUniform2D(float uniformScaleXY);
	void ScaleNonUniform2D(const Vec2& scaleFactorsXY);
	void ScaleUniform3D(float uniformScaleXYZ);
	void ScaleNonUniform3D(const Vec3& scaleFactorsXYZ);
	void TransformBy(const Mat44& arbitraryTransformationToAppend);
	void Negate();
	Mat44 GetTranspose();
	bool IsMatrixOrthoNormal();
	Mat44 GetInvertedForOrthoNormalMatrix();

	static const Mat44		CreateXRotationDegrees(float degreesAboutX);
	static const Mat44		CreateYRotationDegrees(float degreesAboutY);
	static const Mat44		CreateZRotationDegrees(float degreesAboutZ);
	static const Mat44		CreateTranslationXY(const Vec2& translationXY);
	static const Mat44		CreateTranslation3D(const Vec3& translation3D);
	static const Mat44		CreateUniformScaleXY(float uniformScaleXY);
	static const Mat44		CreateNonUniformScaleXY(const Vec2& scaleFactorsXY);
	static const Mat44		CreateUniformScale3D(float uniformScale3D);
	static const Mat44		CreateNonUniformScale3D(const Vec3& scaleFactorsXYZ);
	static const Mat44		CreateOrthographicProjection(const Vec3& min, const Vec3& max);
	static const Mat44		CreatePerspectiveProjection(float fovDegrees, float nearZClip, float farZClip, float aspect);
	static const Mat44		CreateLookAt(const Vec3& start, const Vec3& end, const Vec3& worldUp);
private:
	const Mat44 operator*(const Mat44& rhs) const = delete;
};