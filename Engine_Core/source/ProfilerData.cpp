#include "ProfilerData.h"
#include "Profiler.h"
#include <algorithm>
#include <glm/glm.hpp>

FSectionStatistics::FSectionStatistics(FCategoryNameIdentifier ParentIdentifier, FCategoryNameIdentifier Identifier) : ParentIdentifier(ParentIdentifier), Identifier(Identifier)
{
	Samples.Resize(kMaxSampleSize);
}

void FSectionStatistics::PushSample(float Time)
{
	Samples[CurrentSampleIndex] = Time;
	CurrentSampleIndex++;
	CurrentSampleIndex %= kMaxSampleSize;
	CurrentSampleSize = min(CurrentSampleSize + 1, kMaxSampleSize);
}

void FSectionStatistics::Refresh()
{
	// Calculate median, average and so on.
	TVector<float> SortedList;

	AverageTime = 0.0f;
	MedianTime = 0.0f;
	MinimumTime = FLT_MAX;
	MaximumTime = FLT_MIN;

	for (int i = 0; i < CurrentSampleSize; i++)
	{
		float Value = Samples[i];
		AverageTime += Value;
		MinimumTime = min(Value, MinimumTime);
		MaximumTime = max(Value, MaximumTime);
		SortedList.Add(Value);
	}

	if (CurrentSampleSize != 0)
	{
		AverageTime = AverageTime / (float)CurrentSampleSize;
		std::sort(SortedList.begin(), SortedList.end());
		MedianTime = SortedList[SortedList.Size() / 2];
	}
}

void FSectionStatistics::BeginSample()
{
	CurrentTimer.Start();
}

void FSectionStatistics::EndSample()
{
	CurrentTimer.Restart();
	PushSample(CurrentTimer.GetElapsedTimeMilliseconds());
}

