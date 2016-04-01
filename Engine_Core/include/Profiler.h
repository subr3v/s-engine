#ifndef Profiler_h__
#define Profiler_h__

#include <stack>
#include "ProfilerData.h"

class FProfiler
{
public:
	static FProfiler& Get();

	void ProfileCounter(ConstantString Category, ConstantString Name, bool bResetEveryFrame, s32 ChangeAmount);
	void ResetCounter(ConstantString Category, ConstantString Name, bool bResetEveryFrame);

	void PushSection(ConstantString Category, ConstantString Name);
	void PopSection();

	void OnFrameEnded();

	const TVector<FSectionStatistics>& GetStatistics();
	const TVector<FCounter>& GetCounters();
private:
	FProfiler();
	~FProfiler();

	TVector<FCategoryNameIdentifier> ParentSectionStack;
	TVector<FSectionStatistics> ProfiledSections;
	TVector<FCounter> Counters;

	FCounter& FindCounter(FCategoryNameIdentifier CategoryNameIdentifier, bool bResetEveryFrame);
	FSectionStatistics& FindStatistics(const FCategoryNameIdentifier& ParentName, const FCategoryNameIdentifier& Name);
};

#endif // Profiler_h__
