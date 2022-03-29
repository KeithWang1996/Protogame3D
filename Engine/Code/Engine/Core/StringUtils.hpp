#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
typedef std::vector< std::string >		Strings;
//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string Stringv(char const* format, va_list args);
Strings SplitStringOnDelimiter(const std::string& originalString, char delimiterToSplitOn);
Strings SplitStringOnDelimiter(const std::string& originalString, const std::string& delimiterToSplitOn);
bool CompareTwoStrings(const std::string& stringA, const std::string& stringB);
const std::string ChangeStringTolower(const std::string& originalString);
