#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec4.hpp"

const Rgba8 Rgba8::WHITE = Rgba8();
const Rgba8 Rgba8::BLACK = Rgba8(0, 0, 0, 255);
const Rgba8 Rgba8::RED = Rgba8(255, 0, 0, 255);
const Rgba8 Rgba8::BLUE = Rgba8(0, 0, 255, 255);

Rgba8::Rgba8() 
	:r( 255 )
	,g( 255 )
	,b( 255 )
	,a( 255 )
{}

Rgba8::Rgba8( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}
Rgba8::~Rgba8() {

}

bool Rgba8::operator==(const Rgba8& compare) const
{
	if (r == compare.r && g == compare.g && b == compare.b && a == compare.a)
	{
		return true;
	}
	return false;
}

bool Rgba8::operator!=(const Rgba8& compare) const
{
	if (r != compare.r || g != compare.g || b != compare.b || a != compare.a)
	{
		return true;
	}
	return false;
}

void Rgba8::SetFromText(const char* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	if (strings.size() == 4)
	{
		r = static_cast<unsigned char>(atoi(strings[0].c_str()));
		g = static_cast<unsigned char>(atoi(strings[1].c_str()));
		b = static_cast<unsigned char>(atoi(strings[2].c_str()));
		a = static_cast<unsigned char>(atoi(strings[3].c_str()));
	}
	else if (strings.size() == 3)
	{
		r = static_cast<unsigned char>(atoi(strings[0].c_str()));
		g = static_cast<unsigned char>(atoi(strings[1].c_str()));
		b = static_cast<unsigned char>(atoi(strings[2].c_str()));
		a = 255;
	}
}

Vec4 Rgba8::GetAsVec4() const
{
	float x = (float)r / 255.f;
	float y = (float)g / 255.f;
	float z = (float)b / 255.f;
	float w = (float)a / 255.f;
	return Vec4(x, y, z, w);
}