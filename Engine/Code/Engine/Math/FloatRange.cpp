#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

FloatRange::FloatRange(float minAndMax)
	: minimum(minAndMax)
	, maximum(minAndMax)
{}

FloatRange::FloatRange(float min, float max)
	: minimum(min)
	, maximum(max)
{}

FloatRange::FloatRange(const char* asText)
{
	Strings strings = SplitStringOnDelimiter(asText, '~');
	if (strings.size() == 1)
	{
		minimum = static_cast<float>(atof(strings[0].c_str()));
		maximum = minimum;
	}
	else if (strings.size() == 2)
	{
		minimum = static_cast<float>(atof(strings[0].c_str()));
		maximum = static_cast<float>(atof(strings[1].c_str()));
	}
	else
	{
		minimum = 0.f;
		maximum = 1.f;
	}
}

bool FloatRange::IsInRange(float value) const
{
	if (value < minimum || value > maximum)
	{
		return false;
	}
	return true;
}

bool FloatRange::DoesOverlap(const FloatRange& otherRange) const
{
	if (minimum > otherRange.maximum)
	{
		return false;
	}
	else if (maximum < otherRange.minimum)
	{
		return false;
	}
	return true;
}

std::string FloatRange::GetAsString() const
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

float FloatRange::GetRandomInRange(RandomNumberGenerator& rng) const
{
	return rng.RollRandomFloatInRange(minimum, maximum);
}

void FloatRange::Set(float newMinimum, float newMaximum)
{
	minimum = newMinimum;
	maximum = newMaximum;
}

bool FloatRange::SetFromText(const char* asText)
{
	Strings strings = SplitStringOnDelimiter(asText, '~');
	if (strings.size() == 1)
	{
		minimum = static_cast<float>(atof(strings[0].c_str()));
		maximum = minimum;
		return true;
	}
	else if (strings.size() == 2)
	{
		minimum = static_cast<float>(atof(strings[0].c_str()));
		maximum = static_cast<float>(atof(strings[1].c_str()));
		return true;
	}
	else
	{
		return false;
	}
}