#ifndef MemorySections_h__
#define MemorySections_h__

#include "MemoryTracker.h"

struct FMemorySections
{
public:
	static const FMemorySection& Rendering() 
	{
		static FMemorySection Section("Rendering");
		return Section;
	}

	static const FMemorySection& Resources()
	{
		static FMemorySection Section("Resources");
		return Section;
	}

	static const FMemorySection& Core()
	{
		static FMemorySection Section("Core");
		return Section;
	}

	static const FMemorySection& Utils()
	{
		static FMemorySection Section("Utils");
		return Section;
	}
};

#endif // MemorySections_h__
