#ifndef MemoryTracer_h__
#define MemoryTracer_h__

#include <string>
#include "CoreTypes.h"

struct FMemoryTrackerGuard
{
	FMemoryTrackerGuard();
	~FMemoryTrackerGuard();
};

struct FMemorySection
{
public:
	FMemorySection(const std::string& Name) : Name(Name) { }

	void BeginTracking() const;
	void EndTracking() const;

	void Allocate(int BytesCount);
	void Deallocate();

	bool IsMemoryAllocated() const { return AllocationCount > DeallocationCount; }
	bool IsMemoryCorrupted() const { return DeallocationCount > AllocationCount; }
	int GetTotalAllocatedBytes() const { return TotalAllocatedBytes; }
	int GetAllocationCount() const { return AllocationCount; }
	const std::string& GetName() const { return Name; }

private:
	int TotalAllocatedBytes = 0;
	int AllocationCount = 0;
	int DeallocationCount = 0;
	std::string Name;
};


class FMemoryTracker
{
public:
	inline static FMemorySection* GetCurrentSection()
	{
		return CurrentSection;
	}

	inline static void StartTrackingSection(FMemorySection* Section)
	{
		CurrentSection = Section;
	}

	inline static void StopTrackingSection()
	{
		CurrentSection = nullptr;
	}

	inline static bool IsTrackingMemory()
	{
		return bShouldTrackMemory;
	}

	inline static void StopTrackingMemory()
	{
		bShouldTrackMemory = false;
	}

	inline static void StartTrackingMemory()
	{
		bShouldTrackMemory = true;
	}
private:
	static FMemorySection* CurrentSection;
	static bool bShouldTrackMemory;
};

template <class MemorySectionHolder>
class FMemoryTracked
{
public:
	static void* operator new(SizeType Size)
	{
		MemorySectionHolder::GetSection().Allocate(Size);
		return std::malloc(Size);
	}

	static void operator delete(void* Pointer)
	{
		MemorySectionHolder::GetSection().Deallocate();
		std::free(void* Pointer);
	}
};

#endif // MemoryTracer_h__
