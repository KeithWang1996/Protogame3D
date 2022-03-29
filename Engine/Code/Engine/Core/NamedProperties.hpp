#pragma once
#include <string>
#include <map>
#include <typeinfo>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

class TypedPropertyBase
{
public:
	virtual ~TypedPropertyBase() = default;
	virtual std::string GetAsString() const = 0;
	virtual void const* GetUniqueID() const = 0;

	template <typename T>
	bool Is() const;
	
};

template <typename VALUE_TYPE>
class TypedProperty : public TypedPropertyBase
{
public:
	virtual std::string GetAsString() const final { return ""; }//Not ready for use
	virtual void const* GetUniqueID() const final { return StaticUniqueID(); }
// 	std::string ToStringb(bool value)
// 	{
// 		if (value)
// 		{
// 			return "true";
// 		}
// 		return "false";
// 	}
// 	std::string ToStringv2(Vec2 value)
// 	{
// 		return std::to_string(value.x) + "," + std::to_string(value.y);
// 	}
// 	std::string ToStringv3(Vec3 value)
// 	{
// 		return std::to_string(value.x) + "," + std::to_string(value.y) + std::to_string(value.z);
// 	}
// 	std::string ToStringc(Rgba8 value)
// 	{
// 		return std::to_string(value.r) + "," + std::to_string(value.g) + std::to_string(value.b) + std::to_string(value.a);
// 	}
// 	std::string ToStringf(float value)
// 	{
// 		return std::to_string(value);
// 	}
// 
// 	std::string ToString(VALUE_TYPE value)
// 	{
// 		std::string className = typeid(value).name();
// 		switch (className)
// 		{
// 		case "class float":
// 			return ToStringf(value);
// 			break;
// 		case "class Vec2":
// 			return ToStringv2(value);
// 			break;
// 		case "class Vec3":
// 			return ToStringv3(value);
// 			break;
// 		case "class Rgba8":
// 			return ToStringc(value);
// 			break;
// 		case "class bool":
// 			return ToStringb(value);
// 			break;
// 		default:
// 			return "";
// 			break;
// 		}
// 	}

public:
	VALUE_TYPE m_value;

public:
	static void const* const StaticUniqueID()
	{
		static int s_local = 0;
		return &s_local;
	}
};

template <typename T>
bool TypedPropertyBase::Is() const
{
	return GetUniqueID() == TypedProperty<T>::StaticUniqueID();
}

class NamedProperties
{
public:
	NamedProperties() = default;
	~NamedProperties();

	template <typename T>
	void SetValue(std::string const& keyName, T const& value)
	{
		TypedPropertyBase* base = FindInMap(keyName);
		if (base == nullptr) {
			// doesn't exist, make a new one            
			TypedProperty<T>* prop = new TypedProperty<T>();
			prop->m_value = value;
			m_keyValuePairs[keyName] = prop;
		}
		else {
			// it exists?
			if (base->Is<T>()) {
				TypedProperty<T>* prop = (TypedProperty<T>*)base;
				prop->m_value = value;
			}
			else {
				//if exist, delete
				delete base;
				// doesn't exist, make a new one            
				TypedProperty<T>* prop = new TypedProperty<T>();
				prop->m_value = value;
				m_keyValuePairs[keyName] = prop;
			}
		}
	}


	template <typename T>
	T GetValue(std::string const& keyName, T const& defValue) const
	{
		TypedPropertyBase* base = FindInMap(keyName);
		if (nullptr != base) {
			if (base->Is<T>()) {
				// make sure this is safe!  how....?
				TypedProperty<T>* prop = (TypedProperty<T>*)base;
				return prop->m_value;
			}
			else {
				std::string value = base->GetAsString();
				return defValue;
			}
		}
		else { // failed to find
			return defValue;
		}
	}

	void SetValue(std::string const& keyname, char const* val)
	{
		SetValue<std::string>(keyname, val);
	}

	std::string GetValue(std::string const& keyName, char const* val) const
	{
		return GetValue<std::string>(keyName, val);
	}


private:
	TypedPropertyBase* FindInMap(std::string const& key) const;
	//void operator= (const NamedProperties&);
	//NamedProperties(const NamedProperties&);
	std::map<std::string, TypedPropertyBase*> m_keyValuePairs;
};