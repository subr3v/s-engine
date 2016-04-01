#include "ClassList.h"

FClassList::FClassList(std::initializer_list<const FClassInfo*> Classes) : Count(Classes.size())
{
	for (const FClassInfo* ClassInfo : Classes)
	{
		if (Bitset.Size() < ClassInfo->GetIdentifier() + 1)
		{
			Bitset.Resize(ClassInfo->GetIdentifier() + 1, 0);
		}
		Bitset[ClassInfo->GetIdentifier()] = 1;
	}
}

bool FClassList::operator==(const FClassList& Other) const
{
	return Bitset == Other.Bitset;
}

bool FClassList::HasClass(const FClassInfo* ClassInfo) const
{
	if (Bitset.Size() < ClassInfo->GetIdentifier() + 1)
		return false;
	return Bitset[ClassInfo->GetIdentifier()];
}
