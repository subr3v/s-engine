#include "NetworkBuffer.h"
#include <cassert>

FNetworkBuffer::FNetworkBuffer(u32 BufferSize) : BufferPosition(0)
{
	BufferData.Resize(BufferSize);
}

FNetworkBuffer::~FNetworkBuffer()
{

}

void FNetworkBuffer::CopyData(void* Data, u32 BytesCount)
{
	if (GetSpaceLeft() < BytesCount)
	{
		assert(false);
		return;
	}

	memcpy(BufferData.Data(), Data, BytesCount);
	IncreaseBytesCount(BytesCount);
}

void FNetworkBuffer::IncreaseBytesCount(u32 BytesCount)
{
	BufferPosition += BytesCount;
}

void FNetworkBuffer::DecreaseByteCount(u32 BytesCount)
{
	BufferPosition -= BytesCount;
	memmove(BufferData.Data(), BufferData.Data() + BytesCount, BufferPosition);
}

void FNetworkBuffer::Reset()
{
	BufferPosition = 0;
}

void* FNetworkBuffer::GetCurrentPointerData()
{
	return BufferData.Data() + BufferPosition;
}

void* FNetworkBuffer::GetStartPointerData()
{
	return BufferData.Data();
}

u32 FNetworkBuffer::GetTotalBytes() const
{
	return BufferPosition;
}

u32 FNetworkBuffer::GetSpaceLeft() const
{
	return BufferData.Size() - BufferPosition;
}