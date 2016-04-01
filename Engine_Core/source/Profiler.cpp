#include "Profiler.h"
#include "MemoryTracker.h"
#include <assert.h>
#include "Vector.h"

FCategoryNameIdentifier RootSection = FCategoryNameIdentifier("General", "Root");

FProfiler::FProfiler()
{
	ParentSectionStack.Add(RootSection);
}

FProfiler::~FProfiler()
{

}

FProfiler& FProfiler::Get()
{
	// We don't want to track this allocation.
	FMemoryTrackerGuard MemoryGuard;
	static FProfiler Instance;
	return Instance;
}

FSectionStatistics& FProfiler::FindStatistics(const FCategoryNameIdentifier& ParentName, const FCategoryNameIdentifier& Name)
{
	FMemoryTrackerGuard Guard;

	for (auto& Statistic : ProfiledSections)
	{
		if (Statistic.GetParentName() == ParentName && Statistic.GetSectionName() == Name)
		{
			return Statistic;
		}
	}

	ProfiledSections.Add(FSectionStatistics(ParentName, Name));
	return ProfiledSections.Last();
}

FCounter& FProfiler::FindCounter(FCategoryNameIdentifier Identifier, bool bResetEveryFrame)
{
	FMemoryTrackerGuard Guard;

	for (auto& Counter : Counters)
	{
		if (Counter.GetIdentifier() == Identifier)
		{
			return Counter;
		}
	}

	Counters.Add(FCounter(Identifier, bResetEveryFrame));
	return Counters.Last();
}

const TVector<FSectionStatistics>& FProfiler::GetStatistics()
{
	for (auto& Statistic : ProfiledSections)
	{
		Statistic.Refresh();
	}

	return ProfiledSections;
}

const TVector<FCounter>& FProfiler::GetCounters()
{
	return Counters;
}

void FProfiler::ProfileCounter(ConstantString Category, ConstantString Name, bool bResetEveryFrame, s32 ChangeAmount)
{
	FindCounter(FCategoryNameIdentifier(Category, Name), bResetEveryFrame).Modify(ChangeAmount);
}

void FProfiler::ResetCounter(ConstantString Category, ConstantString Name, bool bResetEveryFrame)
{
	FindCounter(FCategoryNameIdentifier(Category, Name), bResetEveryFrame).Reset();
}

void FProfiler::PushSection(ConstantString Category, ConstantString Name)
{
	FCategoryNameIdentifier ParentName = ParentSectionStack[ParentSectionStack.Size() - 1];
	FSectionStatistics& Statistics = FindStatistics(ParentName, FCategoryNameIdentifier(Category, Name));
	ParentSectionStack.Add(Statistics.GetSectionName());
	Statistics.BeginSample();
}

void FProfiler::PopSection()
{
	// If we have 0 or 1 parents that means that no "begin section" has been called prior to this (parent 0 is always the root). So we just ignore that call.
	if (ParentSectionStack.Size() > 1)
	{
		FCategoryNameIdentifier ParentName = ParentSectionStack[ParentSectionStack.Size() - 2];
		FCategoryNameIdentifier CurrentName = ParentSectionStack[ParentSectionStack.Size() - 1];
		FSectionStatistics& Statistics = FindStatistics(ParentName, CurrentName);
		ParentSectionStack.RemoveAt(ParentSectionStack.Size() - 1);
		Statistics.EndSample();
	}
}

void FProfiler::OnFrameEnded()
{
	for (auto& Counter : Counters)
	{
		if (Counter.DoesResetEveryFrame())
		{
			Counter.Reset();
		}
	}
}

/*
void FProfiler::ShowProfilingResults()
{
	auto ProfilerSections = GetStatistics();
	auto ProfilerCounters = GetCounters();

	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("Profiler", nullptr, ImVec2(800, 600), 1.0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize);

	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Timed sections:");

	for (auto& Section : ProfilerSections)
	{
		ImGui::Text("%s\\%s (%d) : Avg - %.4f ms, Median - %.4f ms, Min - %.4f ms, Max - %.4f ms",
			Section.GetParentName().GetDisplayName().c_str(), Section.GetSectionName().GetDisplayName().c_str(),
			Section.GetSampleCount(), Section.GetAverageTime(), Section.GetMedianTime(),
			Section.GetMinimumTime(), Section.GetMaximumTime());

		//ImGui::PlotLines("", Section.GetSamples().Data(), Section.GetSampleCount(), 0, nullptr, Section.GetMinimumTime(), Section.GetMaximumTime());
	}

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Counters:");

	for (auto& Counter : ProfilerCounters)
	{
		ImGui::Text("%s %s : %d", Counter.GetName().GetCategory().c_str(), Counter.GetName().GetName().c_str(), Counter.GetValue());
	}

	ImGui::End();
}
*/
