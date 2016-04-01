#ifndef NetworkBuffer_h__
#define NetworkBuffer_h__

#include "Vector.h"

class FNetworkBuffer
{
public:
	FNetworkBuffer(u32 BufferSize);
	~FNetworkBuffer();

	void CopyData(void* Data, u32 BytesCount);
	void IncreaseBytesCount(u32 BytesCount);
	void DecreaseByteCount(u32 BytesCount);
	void Reset();

	void* GetCurrentPointerData();
	void* GetStartPointerData();
	u32 GetTotalBytes() const;
	u32 GetSpaceLeft() const;

private:
	TVector<u8> BufferData;
	u32 BufferPosition;
};

#endif // NetworkBuffer_h__
