#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/AABB2.hpp"

int ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return atoi(attributeValue.c_str());
}

char ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return attributeValue[0];
}

bool ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	if (attributeValue == "true" || attributeValue == "True" || attributeValue == "TRUE")
	{
		return true;
	}
	else
	{
		return false;
	}
}

float ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return static_cast<float>(atof(attributeValue.c_str()));
}

Rgba8 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const Rgba8& defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		Rgba8 toreturn;
		toreturn.SetFromText(attributeValue.c_str());
		return toreturn;
	}
}

IntRange ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, IntRange defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		IntRange toReturn;
		toReturn.SetFromText(attributeValue.c_str());
		return toReturn;
	}
}

FloatRange ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, FloatRange defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		FloatRange toReturn;
		toReturn.SetFromText(attributeValue.c_str());
		return toReturn;
	}
}

Vec2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const Vec2& defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		Vec2 toreturn;
		toreturn.SetFromText(attributeValue.c_str());
		return toreturn;
	}
}

AABB2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const AABB2& defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		AABB2 toreturn;
		toreturn.SetFromText(attributeValue.c_str());
		return toreturn;
	}
}

IntVec2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const IntVec2& defaultValue)
{
	std::string attributeValue = ParseXmlAttribute(element, attributeName, static_cast<std::string>(""));
	if (attributeValue == "")
	{
		return defaultValue;
	}
	else
	{
		IntVec2 toreturn;
		toreturn.SetFromText(attributeValue.c_str());
		return toreturn;
	}
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue)
{
	const char* attributeValue = element.Attribute(attributeName);
	if (attributeValue == NULL)
	{
		return defaultValue;
	}
	return element.Attribute(attributeName);
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue)
{
	const char* attributeValue = element.Attribute(attributeName);
	if (attributeValue == NULL)
	{
		return defaultValue;
	}
	return element.Attribute(attributeName);
}

Strings ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const Strings& defaultValues)
{
	std::string defaultString = "";
	if (defaultValues.size() > 0)
	{
		defaultString = defaultValues[0];
		for (int stringIndex = 1; stringIndex < defaultValues.size(); ++stringIndex)
		{
			defaultString += "," + defaultValues[stringIndex];
		}
	}
	std::string attributeValue = ParseXmlAttribute(element, attributeName, defaultString);
	return SplitStringOnDelimiter(attributeValue, ',');
}
