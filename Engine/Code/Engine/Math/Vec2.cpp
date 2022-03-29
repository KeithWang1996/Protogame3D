#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string>
//#include "Engine/Core/EngineCommon.hpp"

const Vec2 Vec2::ZERO = Vec2(0.f, 0.f);
const Vec2 Vec2::ONE = Vec2(1.f, 1.f);

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( 123.f )
	, y( 456.f )
{
	x = copy.x;
	y = copy.y;
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( 123.f )
	, y( 456.f )
{
	x = initialX;
	y = initialY;
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( uniformScale * x , uniformScale * y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	if (inverseScale > 0)
	{
		return Vec2(x / inverseScale, y / inverseScale);
	}
	return *this;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	if (uniformDivisor != 0)
	{
		x /= uniformDivisor;
		y /= uniformDivisor;
	}
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( uniformScale * vecToScale.x,  uniformScale * vecToScale.y);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return false;
	return true;
}

const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
	float x = length * cosf(directionRadians);
	float y = length * sinf(directionRadians);
	return Vec2(x, y);
}
const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
	float x = length * CosDegrees(directionDegrees);
	float y = length * SinDegrees(directionDegrees);
	return Vec2(x, y);
}

//Accessors
float Vec2::GetLength() const
{
	return sqrtf(GetLengthSquared());
}
float Vec2::GetLengthSquared() const
{
	return x * x + y * y;
}
float Vec2::GetAngleRadians() const
{
	return atan2f(y, x);
}
float Vec2::GetAngleDegrees() const
{
	float degrees = Atan2Degrees(y, x);
	while (degrees < 0.f)
	{
		degrees += 360.f;
	}
	while (degrees > 360.f)
	{
		degrees -= 360.f;
	}
	return degrees;
}
const Vec2 Vec2::GetRotated90Degrees() const
{
	return GetRotatedDegrees(90.f);
}
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return GetRotatedDegrees(-90.f);
}
const Vec2 Vec2::GetRotatedRadians(float deltaRadians) const
{
	float length = GetLength();
	float theta = GetAngleRadians();
	theta += deltaRadians;
	float x_rotated = length * cosf(theta);
	float y_rotated = length * sinf(theta);

	return Vec2(x_rotated, y_rotated);
}
const Vec2 Vec2::GetRotatedDegrees(float deltaDegrees) const
{
	float length = GetLength();
	float theta = GetAngleDegrees();
	theta += deltaDegrees;
	float x_rotated = length * CosDegrees(theta);
	float y_rotated = length * SinDegrees(theta);

	return Vec2(x_rotated, y_rotated);
}
const Vec2 Vec2::GetClamped(float maxLength) const
{
	float length = GetLength();
	if (length <= maxLength)
	{
		return Vec2(x, y);
	}
	float scale = maxLength / length;
	return Vec2(x * scale, y * scale);
}
const Vec2 Vec2::GetNormalized() const
{
	float length = GetLength();
	if (length > 0) {
		float scale = 1.f / length;
		return Vec2(x * scale, y * scale);
	}
	return *this;
}

//Mutators
void Vec2::SetAngleRadians(float newAngleRadians)
{
	float length = GetLength();
	x = length * cosf(newAngleRadians);
	y = length * sinf(newAngleRadians);
}
void Vec2::SetAngleDegrees(float newAngleDegrees)
{
	float length = GetLength();
	x = length * CosDegrees(newAngleDegrees);
	y = length * SinDegrees(newAngleDegrees);
}
void Vec2::SetPolarRadians(float newAngleRadians, float newLength)
{
	x = newLength * cosf(newAngleRadians);
	y = newLength * sinf(newAngleRadians);
}
void Vec2::SetPolarDegrees(float newAngleDegrees, float newLength)
{
	x = newLength * CosDegrees(newAngleDegrees);
	y = newLength * SinDegrees(newAngleDegrees);
}
void Vec2::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() != 2)
	{
		return;
	}
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
}
void Vec2::Rotate90Degrees()
{
	RotateDegrees(90.f);
}
void Vec2::RotateMinus90Degrees()
{
	RotateDegrees(-90.f);
}
void Vec2::RotateRadians(float deltaRadians)
{
	float length = GetLength();
	float theta = GetAngleRadians();
	theta += deltaRadians;
	x = length * cosf(theta);
	y = length * sinf(theta);
}
void Vec2::RotateDegrees(float deltaDegrees)
{
	float length = GetLength();
	float theta = GetAngleDegrees();
	theta += deltaDegrees;
	x = length * CosDegrees(theta);
	y = length * SinDegrees(theta);
}
void Vec2::SetLength(float newLength)
{
	float length = GetLength();
	if (length > 0)
	{
		float scale = newLength / length;
		x *= scale;
		y *= scale;
	}

}
void Vec2::ClampLength(float maxLength)
{
	float length = GetLength();
	if (length <= maxLength)
	{
		return;
	}
	float scale = maxLength / length;
	x *= scale;
	y *= scale;
}
void Vec2::Normalize()
{
	float length = GetLength();
	if (length > 0)
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
	}
}
float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();
	Normalize();
	return length;
}

void Vec2::Reflect(const Vec2& normal)
{
	Vec2 vecN = GetProjectedOnto2D(*this, normal);
	*this -= 2 * vecN;
}

const Vec2 Vec2::GetReflected(const Vec2& normal) const
{
	Vec2 vecN = GetProjectedOnto2D(*this, normal);
	return (*this - 2 * vecN);
}

std::string Vec2::ToString() const
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}