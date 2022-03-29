#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <iostream>

IntRange::IntRange(int minAndMax)
	: minimum(minAndMax)
	, maximum(minAndMax)
{}

IntRange::IntRange(int min, int max)
	: minimum(min)
	, maximum(max)
{}

IntRange::IntRange(const char* asText)
{
	Strings strings = SplitStringOnDelimiter(asText, '~');
	if (strings.size() == 1)
	{
		minimum = static_cast<int>(atoi(strings[0].c_str()));
		maximum = minimum;
	}
	else if (strings.size() == 2)
	{
		minimum = static_cast<int>(atoi(strings[0].c_str()));
		maximum = static_cast<int>(atoi(strings[1].c_str()));
	}
	else
	{
		minimum = 0;
		maximum = 0;
	}
}

bool IntRange::IsInRange(int value) const
{
	if (value < minimum || value > maximum)
	{
		return false;
	}
	return true;
}

bool IntRange::DoesOverLap(const IntRange& otherRange) const
{
	if (minimum > otherRange.maximum)
	{
		return true;
	}
	else if (maximum < otherRange.minimum)
	{
		return true;
	}
	return false;
}

std::string IntRange::GetAsString() const
{
	if (maximum == minimum)
	{
		return std::to_string(minimum);
	}
	else
	{
		return std::to_string(minimum) + "~" + std::to_string(maximum);
	}
}

int IntRange::GetRandomInRange(RandomNumberGenerator& rng) const
{
	return rng.RollRandomIntInRange(minimum, maximum);
}

void IntRange::Set(int newMinimum, int newMaximum)
{
	minimum = newMinimum;
	maximum = newMaximum;
}
bool IntRange::SetFromText(const char* asText)
{
	Strings strings = SplitStringOnDelimiter(asText, '~');
	if (strings.size() == 1)
	{
		minimum = static_cast<int>(atoi(strings[0].c_str()));
		maximum = minimum;
		return true;
	}
	else if (strings.size() == 2)
	{
		minimum = (int)atoi(strings[0].c_str());
		maximum = (int)atoi(strings[1].c_str());
		return true;
	}
	else
	{
		return false;
	}
}