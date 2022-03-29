#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

const Mat44 Mat44::IDENTITY = Mat44();
float mat_conversion[] = {
				0.f,	0.f,	-1.f,	0.f,
				-1.f,	0.f,	0.f,	0.f, 
				0.f,	1.f,	0.f,	0.f,
				0.f,	0.f,	0.f,	1.f
};
const Mat44 Mat44::CONVERSION_NZ_NX_Y = Mat44(mat_conversion);
Mat44::Mat44(float* sixteenValuesBasisMajor)
{
	float* m_sixteenValuesBasisMajor = &Ix;
	for (int indexFloats = 0; indexFloats < 16; ++indexFloats)
	{
		m_sixteenValuesBasisMajor[indexFloats] = sixteenValuesBasisMajor[indexFloats];
	}
}

Mat44::Mat44(Vec2& iBasis2D, Vec2& jBasis2D, Vec2& translation2D)
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;

	Tx = translation2D.x;
	Ty = translation2D.y;
}

Mat44::Mat44(Vec3& iBasis3D, Vec3& jBasis3D, Vec3& kBasis3D, Vec3& translation3D)
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

Mat44::Mat44(Vec4& iBasisHomogeneous, Vec4& jBasisHomogeneous, Vec4& kBasisHomogeneous, Vec4& translationHomogeneous)
{
	Ix = iBasisHomogeneous.x;
	Iy = iBasisHomogeneous.y;
	Iz = iBasisHomogeneous.z;
	Iw = iBasisHomogeneous.w;

	Jx = jBasisHomogeneous.x;
	Jy = jBasisHomogeneous.y;
	Jz = jBasisHomogeneous.z;
	Jw = jBasisHomogeneous.w;

	Kx = kBasisHomogeneous.x;
	Ky = kBasisHomogeneous.y;
	Kz = kBasisHomogeneous.z;
	Kw = kBasisHomogeneous.w;

	Tx = translationHomogeneous.x;
	Ty = translationHomogeneous.y;
	Tz = translationHomogeneous.z;
	Tw = translationHomogeneous.w;
}

const Vec2 Mat44::TransformVector2D(const Vec2& vectorQuantity) const
{
	float x = Ix * vectorQuantity.x + Jx * vectorQuantity.y;
	float y = Iy * vectorQuantity.x + Jy * vectorQuantity.y;

	return Vec2(x,y);
}

const Vec3 Mat44::TransformVector3D(const Vec3& vectorQuantity) const
{
	float x = Ix * vectorQuantity.x + Jx * vectorQuantity.y + Kx * vectorQuantity.z;
	float y = Iy * vectorQuantity.x + Jy * vectorQuantity.y + Ky * vectorQuantity.z;
	float z = Iz * vectorQuantity.x + Jz * vectorQuantity.y + Kz * vectorQuantity.z;

	return Vec3(x, y, z);
}
const Vec2 Mat44::TransformPosition2D(const Vec2& position) const
{
	float x = Ix * position.x + Jx * position.y + Tx;
	float y = Iy * position.x + Jy * position.y + Ty;

	return Vec2(x, y);
}
const Vec3 Mat44::TransformPosition3D(const Vec3& position) const
{
	float x = Ix * position.x + Jx * position.y + Kx * position.z + Tx;
	float y = Iy * position.x + Jy * position.y + Ky * position.z + Ty;
	float z = Iz * position.x + Jz * position.y + Kz * position.z + Tz;

	return Vec3(x, y, z);
}
const Vec4 Mat44::TransformHomogeneousPosition3D(const Vec4& homogeneousPoint) const
{
	float x = Ix * homogeneousPoint.x + Jx * homogeneousPoint.y + Kx * homogeneousPoint.z + Tx * homogeneousPoint.w;
	float y = Iy * homogeneousPoint.x + Jy * homogeneousPoint.y + Ky * homogeneousPoint.z + Ty * homogeneousPoint.w;
	float z = Iz * homogeneousPoint.x + Jz * homogeneousPoint.y + Kz * homogeneousPoint.z + Tz * homogeneousPoint.w;
	float w = Iw * homogeneousPoint.x + Jw * homogeneousPoint.y + Kw * homogeneousPoint.z + Tw * homogeneousPoint.w;

	return Vec4(x, y, z, w);
}

const Vec2 Mat44::GetIBasis2D() const
{
	return Vec2(Ix, Iy);
}

const Vec2 Mat44::GetJBasis2D() const
{
	return Vec2(Jx, Jy);
}

const Vec2 Mat44::GetTranslation2D() const
{
	return Vec2(Tx, Ty);
}

const Vec3 Mat44::GetIBasis3D() const
{
	return Vec3(Ix, Iy, Iz);
}

const Vec3 Mat44::GetJBasis3D() const
{
	return Vec3(Jx, Jy, Jz);
}

const Vec3 Mat44::GetKBasis3D() const
{
	return Vec3(Kx, Ky, Kz);
}

const Vec3 Mat44::GetTranslation3D() const
{
	return Vec3(Tx, Ty, Tz);
}

const Vec4 Mat44::GetIBasis4D() const
{
	return Vec4(Ix, Iy, Iz, Iw);
}

const Vec4 Mat44::GetJBasis4D() const
{
	return Vec4(Jx, Jy, Jz, Jw);
}

const Vec4 Mat44::GetKBasis4D() const
{
	return Vec4(Kx, Ky, Kz, Kw);
}

const Vec4 Mat44::GetTranslation4D() const
{
	return Vec4(Tx, Ty, Tz, Tw);
}

void Mat44::SetTranslation2D(const Vec2& translation2D)
{
	Tx = translation2D.x;
	Ty = translation2D.y;
}

void Mat44::SetTranslation3D(const Vec3& translation3D)
{
	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

void Mat44::SetBasisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D)
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;
}

void Mat44::SetBasisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D)
{
	SetBasisVectors2D(iBasis2D, jBasis2D);

	SetTranslation2D(translation2D);
}

void Mat44::SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D)
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;
}

void Mat44::SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D)
{
	SetBasisVectors3D(iBasis3D, jBasis3D, kBasis3D);

	SetTranslation3D(translation3D);
}

void Mat44::SetBasisVectors4D(const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D)
{
	Ix = iBasis4D.x;
	Iy = iBasis4D.y;
	Iz = iBasis4D.z;
	Iw = iBasis4D.w;

	Jx = jBasis4D.x;
	Jy = jBasis4D.y;
	Jz = jBasis4D.z;
	Jw = jBasis4D.w;

	Kx = kBasis4D.x;
	Ky = kBasis4D.y;
	Kz = kBasis4D.z;
	Kw = kBasis4D.w;

	Tx = translation4D.x;
	Ty = translation4D.y;
	Tz = translation4D.z;
	Tw = translation4D.w;
}

void Mat44::RotationXDegrees(float degreeAboutX)
{
	TransformBy(Mat44::CreateXRotationDegrees(degreeAboutX));
}

void Mat44::RotationYDegrees(float degreeAboutY)
{
	TransformBy(Mat44::CreateYRotationDegrees(degreeAboutY));
}

void Mat44::RotationZDegrees(float degreeAboutZ)
{
	TransformBy(Mat44::CreateZRotationDegrees(degreeAboutZ));
}

void Mat44::Translate2D(const Vec2& translationXY)
{
	TransformBy(Mat44::CreateTranslationXY(translationXY));
}

void Mat44::Translate3D(const Vec3& translation3D)
{
	TransformBy(Mat44::CreateTranslation3D(translation3D));
}

void Mat44::ScaleUniform2D(float uniformScaleXY)
{
	TransformBy(Mat44::CreateUniformScaleXY(uniformScaleXY));
}

void Mat44::ScaleNonUniform2D(const Vec2& scaleFactorsXY)
{
	TransformBy(Mat44::CreateNonUniformScaleXY(scaleFactorsXY));
}

void Mat44::ScaleUniform3D(float uniformScaleXYZ)
{
	TransformBy(Mat44::CreateUniformScale3D(uniformScaleXYZ));
}

void Mat44::ScaleNonUniform3D(const Vec3& scaleFactorsXYZ)
{
	TransformBy(Mat44::CreateNonUniformScale3D(scaleFactorsXYZ));
}

void Mat44::TransformBy(const Mat44& arbitraryTransformationToAppend)
{
	Mat44 thisCopy = Mat44(this->GetAsFloatArray());

	Ix = arbitraryTransformationToAppend.Ix * thisCopy.Ix + arbitraryTransformationToAppend.Iy * thisCopy.Jx + arbitraryTransformationToAppend.Iz * thisCopy.Kx + arbitraryTransformationToAppend.Iw * thisCopy.Tx;
	Iy = arbitraryTransformationToAppend.Ix * thisCopy.Iy + arbitraryTransformationToAppend.Iy * thisCopy.Jy + arbitraryTransformationToAppend.Iz * thisCopy.Ky + arbitraryTransformationToAppend.Iw * thisCopy.Ty;
	Iz = arbitraryTransformationToAppend.Ix * thisCopy.Iz + arbitraryTransformationToAppend.Iy * thisCopy.Jz + arbitraryTransformationToAppend.Iz * thisCopy.Kz + arbitraryTransformationToAppend.Iw * thisCopy.Tz;
	Iw = arbitraryTransformationToAppend.Ix * thisCopy.Iw + arbitraryTransformationToAppend.Iy * thisCopy.Jw + arbitraryTransformationToAppend.Iz * thisCopy.Kw + arbitraryTransformationToAppend.Iw * thisCopy.Tw;

	Jx = arbitraryTransformationToAppend.Jx * thisCopy.Ix + arbitraryTransformationToAppend.Jy * thisCopy.Jx + arbitraryTransformationToAppend.Jz * thisCopy.Kx + arbitraryTransformationToAppend.Jw * thisCopy.Tx;
	Jy = arbitraryTransformationToAppend.Jx * thisCopy.Iy + arbitraryTransformationToAppend.Jy * thisCopy.Jy + arbitraryTransformationToAppend.Jz * thisCopy.Ky + arbitraryTransformationToAppend.Jw * thisCopy.Ty;
	Jz = arbitraryTransformationToAppend.Jx * thisCopy.Iz + arbitraryTransformationToAppend.Jy * thisCopy.Jz + arbitraryTransformationToAppend.Jz * thisCopy.Kz + arbitraryTransformationToAppend.Jw * thisCopy.Tz;
	Jw = arbitraryTransformationToAppend.Jx * thisCopy.Iw + arbitraryTransformationToAppend.Jy * thisCopy.Jw + arbitraryTransformationToAppend.Jz * thisCopy.Kw + arbitraryTransformationToAppend.Jw * thisCopy.Tw;

	Kx = arbitraryTransformationToAppend.Kx * thisCopy.Ix + arbitraryTransformationToAppend.Ky * thisCopy.Jx + arbitraryTransformationToAppend.Kz * thisCopy.Kx + arbitraryTransformationToAppend.Kw * thisCopy.Tx;
	Ky = arbitraryTransformationToAppend.Kx * thisCopy.Iy + arbitraryTransformationToAppend.Ky * thisCopy.Jy + arbitraryTransformationToAppend.Kz * thisCopy.Ky + arbitraryTransformationToAppend.Kw * thisCopy.Ty;
	Kz = arbitraryTransformationToAppend.Kx * thisCopy.Iz + arbitraryTransformationToAppend.Ky * thisCopy.Jz + arbitraryTransformationToAppend.Kz * thisCopy.Kz + arbitraryTransformationToAppend.Kw * thisCopy.Tz;
	Kw = arbitraryTransformationToAppend.Kx * thisCopy.Iw + arbitraryTransformationToAppend.Ky * thisCopy.Jw + arbitraryTransformationToAppend.Kz * thisCopy.Kw + arbitraryTransformationToAppend.Kw * thisCopy.Tw;

	Tx = arbitraryTransformationToAppend.Tx * thisCopy.Ix + arbitraryTransformationToAppend.Ty * thisCopy.Jx + arbitraryTransformationToAppend.Tz * thisCopy.Kx + arbitraryTransformationToAppend.Tw * thisCopy.Tx;
	Ty = arbitraryTransformationToAppend.Tx * thisCopy.Iy + arbitraryTransformationToAppend.Ty * thisCopy.Jy + arbitraryTransformationToAppend.Tz * thisCopy.Ky + arbitraryTransformationToAppend.Tw * thisCopy.Ty;
	Tz = arbitraryTransformationToAppend.Tx * thisCopy.Iz + arbitraryTransformationToAppend.Ty * thisCopy.Jz + arbitraryTransformationToAppend.Tz * thisCopy.Kz + arbitraryTransformationToAppend.Tw * thisCopy.Tz;
	Tw = arbitraryTransformationToAppend.Tx * thisCopy.Iw + arbitraryTransformationToAppend.Ty * thisCopy.Jw + arbitraryTransformationToAppend.Tz * thisCopy.Kw + arbitraryTransformationToAppend.Tw * thisCopy.Tw;
}

void Mat44::Negate()
{
	float* m_sixteenValuesBasisMajor = &Ix;
	for (int indexFloats = 0; indexFloats < 16; ++indexFloats)
	{
		m_sixteenValuesBasisMajor[indexFloats] = -m_sixteenValuesBasisMajor[indexFloats];
	}
}

Mat44 Mat44::GetTranspose()
{
	Mat44 thisCopy = Mat44(this->GetAsFloatArray());
	thisCopy.Iy = Jx;
	thisCopy.Iz = Kx;
	thisCopy.Iw = Tx;

	thisCopy.Jx = Iy;
	thisCopy.Jz = Ky;
	thisCopy.Jw = Ty;

	thisCopy.Kx = Iz;
	thisCopy.Ky = Jz;
	thisCopy.Kw = Tz;

	thisCopy.Tx = Iw;
	thisCopy.Ty = Jw;
	thisCopy.Tz = Kw;

	return thisCopy;
}

bool Mat44::IsMatrixOrthoNormal()
{
	//if have time
	//TODO: Use definition for checking
	Mat44 transposed = GetTranspose();
	transposed.TransformBy(*this);
	const float* elementsI = (Mat44::IDENTITY).GetAsFloatArray();
	const float* elements = transposed.GetAsFloatArray();
	for (int i = 0; i < 16; ++i)
	{
		if (abs(elements[i] - elementsI[i]) > 0.001f)
		{
			return false;
		}
	}
	return true;
	
}

Mat44 Mat44::GetInvertedForOrthoNormalMatrix()
{
	//To be changed
	Mat44 inverse = *this;
	inverse.SetTranslation3D(Vec3(0.f, 0.f, 0.f));
	if (!inverse.IsMatrixOrthoNormal())
	{
		return Mat44::IDENTITY;
	}
	inverse = inverse.GetTranspose();

	Vec3 translation = Vec3(Tx, Ty, Tz);
	//inverse.SetTranslation3D(translation);
	inverse.TransformBy(CreateTranslation3D(translation));
	return inverse;
}

const Mat44 Mat44::CreateXRotationDegrees(float degreesAboutX)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Jy = CosDegrees(degreesAboutX);
	defaultMatrix.Ky = -SinDegrees(degreesAboutX);
	defaultMatrix.Jz = SinDegrees(degreesAboutX);
	defaultMatrix.Kz = CosDegrees(degreesAboutX);

	return defaultMatrix;
}

const Mat44 Mat44::CreateYRotationDegrees(float degreesAboutY)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = CosDegrees(degreesAboutY);
	defaultMatrix.Kx = SinDegrees(degreesAboutY);
	defaultMatrix.Iz = -SinDegrees(degreesAboutY);
	defaultMatrix.Kz = CosDegrees(degreesAboutY);

	return defaultMatrix;
}

const Mat44 Mat44::CreateZRotationDegrees(float degreesAboutZ)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = CosDegrees(degreesAboutZ);
	defaultMatrix.Jx = -SinDegrees(degreesAboutZ);
	defaultMatrix.Iy = SinDegrees(degreesAboutZ);
	defaultMatrix.Jy = CosDegrees(degreesAboutZ);

	return defaultMatrix;
}

const Mat44 Mat44::CreateTranslationXY(const Vec2& translationXY)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Tx = translationXY.x;
	defaultMatrix.Ty = translationXY.y;

	return defaultMatrix;
}

const Mat44 Mat44::CreateTranslation3D(const Vec3& translation3D)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Tx = translation3D.x;
	defaultMatrix.Ty = translation3D.y;
	defaultMatrix.Tz = translation3D.z;

	return defaultMatrix;
}

const Mat44 Mat44::CreateUniformScaleXY(float uniformScaleXY)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = uniformScaleXY;
	defaultMatrix.Jy = uniformScaleXY;

	return defaultMatrix;
}

const Mat44 Mat44::CreateNonUniformScaleXY(const Vec2& scaleFactorsXY)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = scaleFactorsXY.x;
	defaultMatrix.Jy = scaleFactorsXY.y;

	return defaultMatrix;
}

const Mat44 Mat44::CreateUniformScale3D(float uniformScale3D)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = uniformScale3D;
	defaultMatrix.Jy = uniformScale3D;
	defaultMatrix.Kz = uniformScale3D;

	return defaultMatrix;
}

const Mat44 Mat44::CreateNonUniformScale3D(const Vec3& scaleFactorsXYZ)
{
	Mat44 defaultMatrix = Mat44::IDENTITY;
	defaultMatrix.Ix = scaleFactorsXYZ.x;
	defaultMatrix.Jy = scaleFactorsXYZ.y;
	defaultMatrix.Kz = scaleFactorsXYZ.z;

	return defaultMatrix;
}

const Mat44	Mat44::CreateOrthographicProjection(const Vec3& min, const Vec3& max)
{
	//think of x
	//min.x, max.x->(-1,1)

	//ndc.x = (x - min.x) / (max.x - min.x) * (1 - (-1.0f)) + (-1);
	//ndc.x = x / (max.x - min.x) - (min.x / (max.x - min.x)) * 2.0f -1.0f;
	//a = 1.0f / (max.x - min.x)
	//b = (-2.0f * min.x - max.x + min.x) / (max.x - min.x)
	//	= -(max.x + min.x) / (max.x - min.x)

	// min.z, max.z ->(0, 1)
	//ndc.x = (x - min.x) / (max.x - min.x) * (1 - (-0.0f)) + (-0);
	//ndc.x = x / (max.x - min.x) - (min.x / (max.x - min.x)) * 1.0f;
	//a = 1.0f / (max.x - min.x)
	//b = -min.x / (max.x - min.x)
	Vec3 diff = max - min;
	Vec3 sum = max + min;
	float mat[] = {
		2.0f / diff.x,			0.0f,					0.0f,					0.0f,
		0.0f,					2.0f / diff.y,			0.0f,					0.0f,
		0.0f,					0.0f,					1.0f / diff.z,			0.0f,
		- sum.x / diff.x,		- sum.y / diff.y,		- min.z / diff.z,		1.0f
	};
	return Mat44(mat);
}

const Mat44 Mat44::CreatePerspectiveProjection(float fovDegrees, float nearZClip, float farZClip, float aspect)
{
	float height = 1.0f / tanf(ConvertDegreesToRadians(fovDegrees * .5f));
	float zrange = farZClip - nearZClip;
	float q = 1.0f / zrange;

	float mat[] = {
		height / aspect,			0.f,				0.f,								 0.f,
				 0.f,				height,				0.f,								 0.f,
				 0.f,				0.f,				-farZClip * q,						-1.f,
				 0.f,				0.f,				nearZClip * farZClip * q,			 0.f
	};
	return Mat44(mat);
}

const Mat44	Mat44::CreateLookAt(const Vec3& start, const Vec3& end, const Vec3& worldUp)
{
	Vec3 forward = (end - start).GetNormalized();
	Vec3 right;
	if (GetDistance3D(forward, worldUp) < 0.01f)
	{
		right = Vec3(0.f, -1.f, 0.f);
	}
	else
	{
		right = CrossProduct3D(forward, worldUp).GetNormalized();
	}
	Vec3 up = CrossProduct3D(right, forward);
	Mat44 lookAt;
	lookAt.SetBasisVectors3D(right, up, -forward);
	lookAt.SetTranslation3D(start);
	return lookAt;
}