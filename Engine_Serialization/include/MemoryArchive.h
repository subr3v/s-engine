#ifndef MemoryArchives_h__
#define MemoryArchives_h__

#include "Archive.h"
#include <cassert>

class FMemoryBufferWriteArchive : public IArchive
{
public:
	FMemoryBufferWriteArchive(TVector<u8>& Memory, u32 StartOffset = 0) : MemoryBuffer(Memory)
	{
		PositionPointer = Memory.Data() + StartOffset;
		CurrentByteCount = StartOffset;
	}

	virtual void Write(const void* Data, u32 ByteSize) override
	{
		if (CurrentByteCount + ByteSize >= MemoryBuffer.Size())
		{
			MemoryBuffer.Resize(CurrentByteCount + ByteSize);
			PositionPointer = MemoryBuffer.Data() + CurrentByteCount;
		}
		memcpy(PositionPointer, Data, ByteSize);
		PositionPointer += ByteSize;
		CurrentByteCount += ByteSize;
	}

	virtual void Read(void* Data, u32 ByteSize) override
	{
		assert(false); // Not allowed to read from this kind of stream.
	}

private:
	TVector<u8>& MemoryBuffer;
	u8* PositionPointer;
	s32 CurrentByteCount;
};

class FMemoryWriteArchive : public IArchive
{
public:
	FMemoryWriteArchive(void* Memory, u32 MemorySize, u32 StartOffset = 0) : MemorySize(MemorySize), CurrentByteCount(StartOffset)
	{
		PositionPointer = (u8*)Memory + StartOffset;
	}

	virtual void Write(const void* Data, u32 ByteSize) override
	{
		if (CurrentByteCount + ByteSize >= MemorySize)
		{
			assert(0);
			return;
		}
		memcpy(PositionPointer, Data, ByteSize);
		PositionPointer += ByteSize;
		CurrentByteCount += ByteSize;
	}

	virtual void Read(void* Data, u32 ByteSize) override
	{
		assert(false); // Not allowed to read from this kind of stream.
	}

private:
	u8* PositionPointer;
	u32 MemorySize;
	s32 CurrentByteCount;
};

class FMemoryReadArchive : public IArchive
{
public:
	FMemoryReadArchive(const void* Memory, u32 StartOffset = 0)
	{
		PositionPointer = (u8*)Memory + StartOffset;
	}

	virtual void Write(const void* Data, u32 ByteSize) override
	{
		assert(0); // Not allowed to write from this kind of stream
	}

	virtual void Read(void* Data, u32 ByteSize) override
	{
		memcpy(Data, PositionPointer, ByteSize);
		PositionPointer += ByteSize;
	}

private:
	const u8* PositionPointer;
};

#endif // MemoryArchives_h__
