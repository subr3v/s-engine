#ifndef ClassList_h__
#define ClassList_h__

#include "ClassType.h"

class FClassList
{
public:
	FClassList(std::initializer_list<const FClassInfo*> Classes);

	bool operator==(const FClassList& Other) const;
	bool HasClass(const FClassInfo* ClassInfo) const;

	int32 GetCount() const { return Count; }
private:
	TVector<bool> Bitset;
	int32 Count;
};

#endif // ClassList_h__
