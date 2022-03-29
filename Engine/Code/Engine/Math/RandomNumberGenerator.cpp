#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/Vec2.hpp"
#include <stdlib.h>

int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	return Get1dNoiseUint(m_position++, m_seed) % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int deltaRange = maxInclusive - minInclusive + 1;
	return Get1dNoiseUint(m_position++, m_seed) % deltaRange + minInclusive;
}

float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
{
	constexpr double scale = 1.0 / (double)0xFFFFFFFF;
	return (float)scale * (float)Get1dNoiseUint(m_position++, m_seed);
}

float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
{
	constexpr double scale = 1.0 / ((double)0xFFFFFFFF + 1.0);
	return (float)scale * (float)Get1dNoiseUint(m_position++, m_seed);
}

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	constexpr double scale = 1.0 / ((double)0xFFFFFFFF + 1.0);
	return (float)scale * maxNotInclusive * (float)Get1dNoiseUint(m_position++, m_seed);
}

float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	constexpr double scale = 1.0 / (double)0xFFFFFFFF;
	return minInclusive + (float)scale * (float)Get1dNoiseUint(m_position++, m_seed) * (maxInclusive - minInclusive);
}

bool RandomNumberGenerator::RollPercentChance(float probabilityOfReturningTrue)
{
	if (RollRandomFloatZeroToAlmostOne() >= probabilityOfReturningTrue)
	{
		return false;
	}
	return true;
}

Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	float randomDegree = RollRandomFloatInRange(0.f, 360.0f);
	Vec2 randomDirection = Vec2::MakeFromPolarDegrees(randomDegree, 1.0f);
	return randomDirection;
}

void RandomNumberGenerator::Reset(unsigned int seed)
{
	m_position = 0;
	m_seed = seed;
}

//dividing is slower than multiplying
