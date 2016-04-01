#include "ClassType.h"
#include <cstdarg>

u32 FClassInfo::IdentifierGenerator = 0;

FClassInfo::FClassInfo(const char* PrintName)
{
	// Note: this is not thread safe at all.
	this->PrintName = PrintName;
	this->Identifier = IdentifierGenerator++;
}

FClassInfo::~FClassInfo()
{

}