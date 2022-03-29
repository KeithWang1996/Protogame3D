#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

const IntVec2 IntVec2::ZERO = IntVec2(0, 0);
const IntVec2 IntVec2::ONE = IntVec2(1, 1);

IntVec2::IntVec2(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}
IntVec2::IntVec2(int initialX, int initialY)
{
	x = initialX;
	y = initialY;
}

float IntVec2::GetLength() const
{
	return sqrtf(static_cast<float>(GetLengthSquared()));
}

int IntVec2::GetLengthSquared() const
{
	return x * x + y * y;
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

float IntVec2::GetOrientationRadians() const
{
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
}
const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y, x);
}
const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y, -x);
}

void IntVec2::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() != 2)
	{
		return;
	}
	x = atoi(strings[0].c_str());
	y = atoi(strings[1].c_str());
}

void IntVec2::Rotate90Degrees()
{
	int temp = x;
	x = -y;
	y = temp;
}
void IntVec2::RotateMinus90Degrees()
{
	int temp = x;
	x = y;
	y = -temp;
}

Vec2 IntVec2::GetAsVec2() const
{
	float fx = static_cast<float>(x);
	float fy = static_cast<float>(y);
	return Vec2(fx, fy);
}

bool IntVec2::operator==(const IntVec2& compare) const
{
	if (x == compare.x && y == compare.y)
	{
		return true;
	}
	return false;
}
bool IntVec2::operator!=(const IntVec2& compare) const
{
	return(!(*this == compare));
}

const IntVec2 IntVec2::operator+(const IntVec2& vecToAdd) const
{
	return IntVec2(x + vecToAdd.x, y + vecToAdd.y);
}

const IntVec2 IntVec2::operator-(const IntVec2& vecToSubtract) const
{
	return IntVec2(x - vecToSubtract.x, y - vecToSubtract.y);
}

const IntVec2 IntVec2::operator-()
{
	return IntVec2(-x, -y);
}

const IntVec2 IntVec2::operator*(int uniformScale) const
{
	return IntVec2(x * uniformScale, y * uniformScale);
}

const IntVec2 IntVec2::operator*(const IntVec2& vecToMultiply) const
{
	return IntVec2(x * vecToMultiply.x, y * vecToMultiply.y);
}

void IntVec2::operator+=(const IntVec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void IntVec2::operator-=(const IntVec2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void IntVec2::operator*=(const int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}