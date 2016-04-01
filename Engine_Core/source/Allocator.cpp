#include "Allocator.h"
#include "MemoryTracker.h"

FMemorySection* FMemoryTracker::CurrentSection = nullptr;
bool FMemoryTracker::bShouldTrackMemory = false;
FAllocator& CurrentDefaultAllocator = FAllocator("Starting_Allocator");

FAllocator& GetDefaultAllocator()
{
	return CurrentDefaultAllocator;
}

void SetDefaultAllocator(FAllocator& Allocator)
{
	CurrentDefaultAllocator = Allocator;
}

void* operator new(SizeType Size)
{
	FMemorySection* CurrentSection = FMemoryTracker::GetCurrentSection();
	if (CurrentSection != nullptr && FMemoryTracker::IsTrackingMemory())
	{
		CurrentSection->Allocate(Size);
	}

	return Engine_MemAlloc(CurrentDefaultAllocator, Size);
}

void operator delete(void* Pointer)
{
	FMemorySection* CurrentSection = FMemoryTracker::GetCurrentSection();
	if (CurrentSection != nullptr && FMemoryTracker::IsTrackingMemory())
	{
		CurrentSection->Deallocate();
	}

	Engine_MemFree(CurrentDefaultAllocator, Pointer);
}

void* operator new[](SizeType Size)
{
	FMemorySection* CurrentSection = FMemoryTracker::GetCurrentSection();
	if (CurrentSection != nullptr && FMemoryTracker::IsTrackingMemory())
	{
		CurrentSection->Allocate(Size);
	}

	return Engine_MemAlloc(CurrentDefaultAllocator, Size);
}

void operator delete[](void* Pointer)
{
	FMemorySection* CurrentSection = FMemoryTracker::GetCurrentSection();
	if (CurrentSection != nullptr && FMemoryTracker::IsTrackingMemory())
	{
		CurrentSection->Deallocate();
	}

	Engine_MemFree(CurrentDefaultAllocator, Pointer);
}