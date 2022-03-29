#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( 123.f )
	, y( 456.f )
	, z( 789.f )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

Vec3::Vec3(const char* text)
{
	SetFromText(text);
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( 123.f )
	, y( 456.f )
	, z( 789.f )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vec3::Vec3(const Vec2& changeFrom, float initialZ)
	: x( 123.f )
	, y( 456.f )
	, z( 789.f )
{
	x = changeFrom.x;
	y = changeFrom.y;
	z = initialZ;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3( -x, -y, -z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( uniformScale * x, uniformScale * y, uniformScale * z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	if (inverseScale != 0)
	{
		return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
	}
	return *this;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	if (uniformDivisor != 0)
	{
		x /= uniformDivisor;
		y /= uniformDivisor;
		z /= uniformDivisor;
	}
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if( x == compare.x && y == compare.y && z == compare.z)
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if( x == compare.x && y == compare.y && z == compare.z )
		return false;
	return true;
}

Vec2 Vec3::xy()
{
	return Vec2(x, y);
}

float Vec3::GetLength() const 
{
	return sqrtf(GetLengthSquared());
}
float Vec3::GetLengthXY() const
{
	return sqrtf(GetLengthXYSquared());
}
float Vec3::GetLengthSquared() const
{
	return x*x + y*y + z*z;
}
float Vec3::GetLengthXYSquared() const
{
	return x*x + y*y;
}
float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}
float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y, x);
}
const Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	float lengthXY = GetLengthXY();
	float theta = GetAngleAboutZRadians();
	theta += deltaRadians;
	float x_rotated = lengthXY * cosf(theta);
	float y_rotated = lengthXY * sinf(theta);

	return Vec3(x_rotated, y_rotated, z);
}
const Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	float lengthXY = GetLengthXY();
	float theta = GetAngleAboutZDegrees();
	theta += deltaDegrees;
	float x_rotated = lengthXY * CosDegrees(theta);
	float y_rotated = lengthXY * SinDegrees(theta);

	return Vec3(x_rotated, y_rotated, z);
}
const Vec3 Vec3::GetClamped( float maxLength ) const
{
	float length = GetLength();
	if (length <= maxLength)
	{
		return Vec3(x, y, z);
	}
	float scale = maxLength / length;
	return Vec3(x * scale, y * scale, z * scale);
}
const Vec3 Vec3::GetNormalized() const
{
	float length = GetLength();
	if (length > 0)
	{
		float scale = 1.f / length;
		return Vec3(x * scale, y * scale, z * scale);
	}
	return *this;
}

const Vec3 Vec3::GetTranslated2D(Vec2 offset) const
{
	return Vec3(x + offset.x, y + offset.y, z);
}

void Vec3::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() != 3)
	{
		return;
	}
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
	z = static_cast<float>(atof(strings[2].c_str()));
}