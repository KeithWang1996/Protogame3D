#include "Engine/Core/NamedStrings.hpp"

void NamedStrings::PopulateFromXmlElementAttributes(const tinyxml2::XMLElement& element)
{
	for (const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute; attribute = attribute->Next())
	{
		m_keyValuePairs[attribute->Name()] = attribute->Value();
	}
}

void NamedStrings::SetValue(const std::string& keyName, const std::string& newValue)
{
// 	if (m_keyValuePairs.find(keyName) != m_keyValuePairs.end())
// 	{
// 		return;
// 	}
	m_keyValuePairs[keyName] = newValue;
}

bool NamedStrings::GetValue(const std::string& keyName, bool defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	if (m_keyValuePairs.at(keyName) == "true" || m_keyValuePairs.at(keyName) == "True" || m_keyValuePairs.at(keyName) == "TRUE")
	{
		return true;
	}
	else if (m_keyValuePairs.at(keyName) == "false" || m_keyValuePairs.at(keyName) == "False" || m_keyValuePairs.at(keyName) == "FALSE")
	{
		return false;
	}
	else
	{
		return defaultValue;
	}
}

int NamedStrings::GetValue(const std::string& keyName, int defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return atoi(m_keyValuePairs.at(keyName).c_str());
}

float NamedStrings::GetValue(const std::string& keyName, float defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return static_cast<float>(atof(m_keyValuePairs.at(keyName).c_str()));
}

std::string NamedStrings::GetValue(const std::string& keyName, std::string defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return m_keyValuePairs.at(keyName);
}

std::string NamedStrings::GetValue(const std::string& keyName, const char* defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return m_keyValuePairs.at(keyName);
}

Rgba8 NamedStrings::GetValue(const std::string& keyName, const Rgba8& defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	Rgba8 toreturn;
	toreturn.SetFromText(m_keyValuePairs.at(keyName).c_str());
	return toreturn;
}

Vec2 NamedStrings::GetValue(const std::string& keyName, const Vec2& defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	Vec2 toreturn;
	toreturn.SetFromText(m_keyValuePairs.at(keyName).c_str());
	return toreturn;
}

Vec3 NamedStrings::GetValue(const std::string& keyName, const Vec3& defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	Vec3 toreturn;
	toreturn.SetFromText(m_keyValuePairs.at(keyName).c_str());
	return toreturn;
}

IntVec2 NamedStrings::GetValue(const std::string& keyName, const IntVec2& defaultValue) const
{
	if (m_keyValuePairs.find(keyName) == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	IntVec2 toreturn;
	toreturn.SetFromText(m_keyValuePairs.at(keyName).c_str());
	return toreturn;
}