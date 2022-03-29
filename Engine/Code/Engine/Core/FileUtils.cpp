#include "Engine/Core/FileUtils.hpp"
#include <io.h>
Strings ReadAllFilesIn(std::string pathAndFormat)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	Strings toReturn;

	if ((hFile = _findfirst(pathAndFormat.c_str(), &c_file)) == -1L)//cannot read
	{
		_findclose(hFile);
		return toReturn;
	}
	
	do {
		toReturn.push_back(std::string(c_file.name));
	} while (_findnext(hFile, &c_file) == 0);
	_findclose(hFile);
	return toReturn;
}