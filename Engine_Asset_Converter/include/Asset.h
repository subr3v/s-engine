#ifndef Asset_h__
#define Asset_h__

#include <string>

class FAsset
{
public:
	FAsset(const std::string& Filename);
	~FAsset();

	const std::string& GetName() const { return Name; }
	const std::string& GetFilename() const { return Filename; }
	const std::string& GetFilenameNoExtension() const { return FilenameNoExtension; }
	const std::string& GetExtension() const { return Extension; }
private:
	std::string Name;
	std::string Filename;
	std::string FilenameNoExtension;
	std::string Extension;
};

#endif // Asset_h__
