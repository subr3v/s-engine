#ifndef Archive_h__
#define Archive_h__

#include "Core.hpp"

class IArchive
{
public:
	virtual ~IArchive() { }

	virtual void Write(const void* Data, u32 ByteSize) = 0;
	virtual void Read(void* Data, u32 ByteSize) = 0;
};

#endif // Archive_h__
