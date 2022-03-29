#pragma once
struct Vec2;
class RandomNumberGenerator
{
public:
// 	RandomNumberGenerator() = default;
// 	explicit RandomNumberGenerator(unsigned int seed);

	//Random ints
	int RollRandomIntLessThan(int maxNotInclusive);
	int RollRandomIntInRange(int minInclusize, int maxInclusive);

	//Random floats
	float RollRandomFloatZeroToOneInclusive();
	float RollRandomFloatZeroToAlmostOne();
	float RollRandomFloatLessThan(float maxNotInclusive);
	float RollRandomFloatInRange(float minInclusive, float maxInclusive);

	//Random bool
	bool RollPercentChance(float probabilityOfReturningTrue);

	//Random Vec2
	Vec2 RollRandomDirection2D();
	//Reset seed
	void Reset(unsigned int seed = 0);
	int GetPosition() { return m_position; }
private:
	int m_position = 0;
	unsigned int m_seed = 0;
};