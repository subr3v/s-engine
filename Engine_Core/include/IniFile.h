#ifndef IniFile_h__
#define IniFile_h__

#include <map>
#include <string>

typedef std::map<std::string, std::string> FIniSection;

class FIni
{
public:
	typedef std::map<std::string, FIniSection> DataType;

	void LoadFromFile(const std::string& Filename);
	void SaveToFile(const std::string& Filename);

	bool GetSection(const std::string& Name, FIniSection** OutSection, bool bCreateIfNotExists);

	DataType& GetData();
private:
	DataType Data;
};


#endif // IniFile_h__
