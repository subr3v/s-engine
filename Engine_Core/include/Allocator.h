#ifndef Allocator_h__
#define Allocator_h__

#include "CoreTypes.h"
#include <stdlib.h>

#define DEFAULT_ALLOCATOR_NAME "Default_Allocator"
// Temporary defines for now
#define DEFAULT_MEM_ALIGNMENT 4
#define DEFAULT_ALLOCATOR FAllocator(DEFAULT_ALLOCATOR_NAME)

#define Engine_MemAlloc(Allocator, NumBytes) (Allocator).Allocate((NumBytes), DEFAULT_MEM_ALIGNMENT)
#define Engine_MemAllocAligned(Allocator, NumBytes, Alignment) (Allocator).Allocate((NumBytes), (Alignment))
#define Engine_MemFree(Allocator, Pointer) (Allocator).Deallocate((void*)(Pointer))

class FAllocator
{
public:
	inline FAllocator(const char* Name);

	inline void* Allocate(uint32 NumBytes, uint32 Alignment);
	inline void Deallocate(void* Memory);

	inline const char* GetName() const;
	inline void SetName(const char* NewName);

private:
#if DEBUG
	const char* Name;
#endif
};

// Implementation of default allocator:
const char* FAllocator::GetName() const
{
#if DEBUG
	return Name;
#else
	return DEFAULT_ALLOCATOR_NAME;
#endif
}

void FAllocator::SetName(const char* NewName)
{
#if DEBUG
	this->Name = NewName;
#endif
}

FAllocator::FAllocator(const char* Name)
{
#if DEBUG
	this->Name = Name;
#endif
}

// Very generic implementation for now
void* FAllocator::Allocate(uint32 NumBytes, uint32 Alignment)
{
	return _aligned_malloc(NumBytes, Alignment);
}

void FAllocator::Deallocate(void* Memory)
{
	_aligned_free(Memory);
}

FAllocator& GetDefaultAllocator();
void SetDefaultAllocator(FAllocator& Allocator);

#endif // Allocator_h__
