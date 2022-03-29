#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
Vec4::Vec4()
	: x(0.f)
	, y(0.f)
	, z(0.f)
	, w(0.f)
{}

Vec4::Vec4(const Vec4& copyFrom)
	: x(123.f)
	, y(456.f)
	, z(789.f)
	, w(0.f)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

Vec4::Vec4(const Vec3& copyFrom, float w)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(copyFrom.z)
	, w(w)
{}

//-----------------------------------------------------------------------------------------------
Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	: x(123.f)
	, y(456.f)
	, z(789.f)
	, w(0.f)
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}

Vec3 Vec4::XYZ() const
{
	return Vec3(x, y, z);
}

void Vec4::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() != 4)
	{
		return;
	}
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
	z = static_cast<float>(atof(strings[2].c_str()));
	w = static_cast<float>(atof(strings[3].c_str()));
}

const Vec4 Vec4::operator-(const Vec4& vecToSubtract) const
{
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}

const Vec4 Vec4::operator-() const
{
	return Vec4(-x, -y, -z, -w);
}

void Vec4:: operator*= (const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}