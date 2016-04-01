#include "MemoryTracker.h"
#include "CoreTypes.h"

void FMemorySection::Allocate(int BytesCount)
{
	TotalAllocatedBytes += BytesCount;
	AllocationCount++;
}

void FMemorySection::Deallocate()
{
	DeallocationCount++;
}

void FMemorySection::BeginTracking() const
{
	FMemoryTracker::StartTrackingSection(const_cast<FMemorySection*>(this));
}

void FMemorySection::EndTracking() const
{
	FMemoryTracker::StopTrackingSection();
}

FMemoryTrackerGuard::FMemoryTrackerGuard()
{
	FMemoryTracker::StopTrackingMemory();
}

FMemoryTrackerGuard::~FMemoryTrackerGuard()
{
	FMemoryTracker::StartTrackingMemory();
}