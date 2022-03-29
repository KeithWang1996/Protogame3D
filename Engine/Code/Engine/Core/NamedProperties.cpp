#include "Engine/Core/NamedProperties.hpp"

NamedProperties::~NamedProperties()
{
	std::map<std::string, TypedPropertyBase*>::iterator it;
	for (it = m_keyValuePairs.begin(); it != m_keyValuePairs.end(); ++it)
	{
		if (it->second != nullptr)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
	m_keyValuePairs.clear();
}

TypedPropertyBase* NamedProperties::FindInMap(std::string const& key) const
{
	auto iter = m_keyValuePairs.find(key);
	if (iter != m_keyValuePairs.end()) {
		return iter->second;
	}
	else {
		return nullptr;
	}
}