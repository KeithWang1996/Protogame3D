#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

const std::string Stringv(char const* format, va_list args)
{
	char buffer[1024];
	vsnprintf_s(buffer, 1024, format, args);
	return buffer;
}

Strings SplitStringOnDelimiter(const std::string& originalString, char delimiterToSplitOn)
{
	Strings splitStrings;
	size_t subStringStartIndex = 0;
	size_t delimiterIndex = 0;
	size_t subStringLength = 0;

	for(;;)
	{
		delimiterIndex = originalString.find(delimiterToSplitOn, subStringStartIndex);
		if (delimiterIndex == std::string::npos)
		{
			subStringLength = originalString.size() - subStringStartIndex;
			std::string substring(originalString, subStringStartIndex, subStringLength);
			splitStrings.push_back(substring);
			break;
		}

		subStringLength = delimiterIndex - subStringStartIndex;
		std::string substring(originalString, subStringStartIndex, subStringLength);
		splitStrings.push_back(substring);
		subStringStartIndex = delimiterIndex + 1;
	}
	return splitStrings;
}

Strings SplitStringOnDelimiter(const std::string& originalString, const std::string& delimiterToSplitOn)
{
	Strings splitStrings;
	size_t subStringStartIndex = 0;
	size_t delimiterIndex = 0;
	size_t subStringLength = 0;

	for (;;)
	{
		delimiterIndex = originalString.find(delimiterToSplitOn, subStringStartIndex);
		if (delimiterIndex == std::string::npos)
		{
			subStringLength = originalString.size() - subStringStartIndex;
			std::string substring(originalString, subStringStartIndex, subStringLength);
			splitStrings.push_back(substring);
			break;
		}

		subStringLength = delimiterIndex - subStringStartIndex;
		std::string substring(originalString, subStringStartIndex, subStringLength);
		splitStrings.push_back(substring);
		subStringStartIndex = delimiterIndex + delimiterToSplitOn.size();
	}
	return splitStrings;
}

bool CompareTwoStrings(const std::string& stringA, const std::string& stringB)
{
	if (stringA.size() != stringB.size())
	{
		return false;
	}
	for (int i = 0; i < stringA.size(); ++i)
	{
		if (tolower(stringA[i]) != tolower(stringB[i]))
		{
			return false;
		}
	}
	return true;
}

const std::string ChangeStringTolower(const std::string& originalString)
{
	std::string result = originalString;
	for (int i = 0; i < result.size(); ++i)
	{
		result[i] = (char)tolower(result[i]);
	}
	return result;
}
