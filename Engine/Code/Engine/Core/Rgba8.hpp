#pragma once
struct Vec4;
struct Rgba8
{
public:
	unsigned char r, g, b, a;
	Rgba8();
	~Rgba8();

	void	SetFromText(const char* text);

	explicit Rgba8( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255 );
	bool		operator==(const Rgba8& compare) const;
	bool		operator!=(const Rgba8& compare) const;
	static const Rgba8 WHITE;
	static const Rgba8 BLACK;
	static const Rgba8 RED;
	static const Rgba8 BLUE;
	Vec4 GetAsVec4() const;
};