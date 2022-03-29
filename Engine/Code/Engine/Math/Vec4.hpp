#pragma once
struct Vec3;
struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	Vec4();
	Vec4(const Vec4& copyFrom);
	Vec4(const Vec3& copyFrom, float w);
	Vec3 XYZ() const;
	void	SetFromText(const char* text);

	const Vec4	operator-(const Vec4& vecToSubtract) const;
	const Vec4	operator-() const;
	void		operator*=(const float uniformScale);
	explicit Vec4(float initialX, float initialY, float initialZ, float initialW);
};