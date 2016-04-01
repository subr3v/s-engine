#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_

#include <windows.h>

class FTimer
{
public:
	bool Start();
	void Restart();
	float GetElapsedTimeSeconds();
	float GetElapsedTimeMilliseconds();
private:
	INT64 Frequency;
	float TicksPerMs;
	INT64 StartTime;
	float FrameTime;
};

#endif