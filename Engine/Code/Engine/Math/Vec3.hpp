#pragma once
#include "Engine/Math/Vec2.hpp"
//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}												// destructor (do nothing) 
	Vec3() {}				  							    // default constructor (do nothing)
	Vec3( const Vec3& copyFrom );							// copy constructor (from another vec3)
	Vec3( const char* text );							// Text constructor
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y)
	explicit Vec3( const Vec2& changeFrom, float initialZ );
	Vec2		xy();
	float		GetLength() const;
	float		GetLengthXY() const;
	float		GetLengthSquared() const;
	float		GetLengthXYSquared() const;
	float		GetAngleAboutZRadians() const;
	float		GetAngleAboutZDegrees() const;
	const Vec3	GetRotatedAboutZRadians( float deltaRadians ) const;
	const Vec3  GetRotatedAboutZDegrees( float deltaDegrees ) const;
	const Vec3	GetClamped( float maxLength ) const;
	const Vec3  GetNormalized() const;
	const Vec3  GetTranslated2D(Vec2 offset) const;
	void		SetFromText(const char* text);

															// Operators (const)
	bool		operator==( const Vec3& compare ) const;		// vec3 == vec3
	bool		operator!=( const Vec3& compare ) const;		// vec3 != vec3
	const Vec3	operator+( const Vec3& vecToAdd ) const;		// vec3 + vec3
	const Vec3	operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
	const Vec3	operator-() const;								// -vec3, i.e. "unary negation"
	const Vec3	operator*( float uniformScale ) const;			// vec3 * float
	const Vec3	operator*( const Vec3& vecToMultiply ) const;	// vec3 * vec3
	const Vec3	operator/( float inverseScale ) const;			// vec3 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec3& vecToAdd );				// vec3 += vec3
	void		operator-=( const Vec3& vecToSubtract );		// vec3 -= vec3
	void		operator*=( const float uniformScale );			// vec3 *= float
	void		operator/=( const float uniformDivisor );		// vec3 /= float
	void		operator=( const Vec3& copyFrom );				// vec3 = vec3

	                                                            // Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3
};


