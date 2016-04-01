#include "timer.h"

bool FTimer::Start()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	if(Frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	TicksPerMs = (float)(Frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	return true;
}


void FTimer::Restart()
{
	INT64 CurrentTime;
	float TimeDifference;

	// Query the current time.
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	// Calculate the difference in time since the last time we queried for the current time.
	TimeDifference = (float)(CurrentTime - StartTime);

	// Calculate the frame time by the time difference over the timer speed resolution.
	FrameTime = TimeDifference / TicksPerMs;

	// Restart the timer.
	StartTime = CurrentTime;

	return;
}

float FTimer::GetElapsedTimeSeconds()
{
	return FrameTime * 0.001f;
}

float FTimer::GetElapsedTimeMilliseconds()
{
	return FrameTime;
}
