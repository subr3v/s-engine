#ifndef Utils_h__
#define Utils_h__

#include <string>
#include <algorithm>

static std::string GetNameFromPath(const std::string& String)
{
	return std::string(std::find_if(String.rbegin(), String.rend(),
		[=](char Character)
	{
		return Character == '\\';
	}).base(), String.end());
}

static std::string RemoveExtension(std::string const& filename)
{
	std::string::const_reverse_iterator pivot = std::find(filename.rbegin(), filename.rend(), '.');
	return pivot == filename.rend() ? filename : std::string(filename.begin(), pivot.base() - 1);
}

#endif // Utils_h__
