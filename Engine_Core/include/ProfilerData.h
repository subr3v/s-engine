#ifndef ProfiledSection_h__
#define ProfiledSection_h__

#include "Vector.h"
#include <string>
#include <functional>
#include "Timer.h"

struct FCategoryNameIdentifier
{
	FCategoryNameIdentifier()
	{

	}

	FCategoryNameIdentifier(ConstantString Category, ConstantString Name) : Category(Category), Name(Name)
	{

	}

	ConstantString Category;
	ConstantString Name;

	bool operator==(const FCategoryNameIdentifier& Other) const
	{
		return strcmp(Category, Other.Category) == 0 && strcmp(Name, Other.Name) == 0;
	}
};

struct FCounter
{
public:
	FCounter(const FCategoryNameIdentifier& Identifier, bool bResetEveryFrame) : Identifier(Identifier), CurrentValue(0), bResetEveryFrame(bResetEveryFrame) { }
	void Modify(s32 Amount) { CurrentValue += Amount; }
	void Reset() { LastValueBeforeReset = CurrentValue; CurrentValue = 0; }

	const FCategoryNameIdentifier& GetIdentifier() const { return Identifier; }
	int GetValue() const { return CurrentValue; }
	int GetLastValue() const { return LastValueBeforeReset; }
	bool DoesResetEveryFrame() const { return bResetEveryFrame; }
private:
	FCategoryNameIdentifier Identifier;
	bool bResetEveryFrame;
	int CurrentValue;
	int LastValueBeforeReset;
};

struct FSectionStatistics
{
public:
	FSectionStatistics(FCategoryNameIdentifier ParentIdentifier, FCategoryNameIdentifier Identifier);

	void BeginSample();
	void EndSample();
	void Refresh();

	const FCategoryNameIdentifier& GetParentName() const { return ParentIdentifier; }
	const FCategoryNameIdentifier& GetSectionName() const { return Identifier; }

	int GetSampleCount() const { return CurrentSampleSize; }
	float GetAverageTime() const { return AverageTime; }
	float GetMinimumTime() const { return MinimumTime; }
	float GetMaximumTime() const { return MaximumTime; }
	float GetMedianTime() const { return MedianTime; }
	const TVector<float>& GetSamples() const { return Samples; }

private:
	void PushSample(float Time);
	FCategoryNameIdentifier ParentIdentifier;
	FCategoryNameIdentifier Identifier;
	FTimer CurrentTimer;
	
	float MedianTime = 0.0f;
	float AverageTime = 0.0f;
	float MinimumTime = 0.0f;
	float MaximumTime = 0.0f;

	// This acts as a ring buffer.
	TVector<float> Samples;
	int CurrentSampleIndex = 0;
	int CurrentSampleSize = 0;
	static const int kMaxSampleSize = 60;
};


#endif // ProfiledSection_h__
