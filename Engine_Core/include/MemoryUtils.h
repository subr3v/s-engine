#ifndef MemoryUtils_h__
#define MemoryUtils_h__

#include <memory>
#include "CoreTypes.h"

template<typename T, SizeType Size>
void ResetArray(T(&Array)[Size]) 
{
	memset(Array, T(), sizeof(T) * Size);
}

class FGraphicsAlignedBuffer
{
public:
	static void* operator new(SizeType Size)
	{
		return _aligned_malloc(Size, 16);
	}

	static void operator delete(void* Pointer)
	{
		_aligned_free(Pointer);
	}

	static void* operator new[](SizeType Size)
	{
		return _aligned_malloc(Size, 16);
	}

	static void operator delete[](void* Pointer)
	{
		_aligned_free(Pointer);
	}
};

bool IsBigEndian();
void SwapBytes(char* Bytes, int BytesCount);

#endif // MemoryUtils_h__
