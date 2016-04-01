#include "Serializer.h"
#include "MemoryUtils.h"

FSerializer::FSerializer(IArchive* Archive, bool bIsLoading, bool bOwnArchive, bool bEndianIndependent  /*= false*/) : bIsLoading(bIsLoading), Archive(Archive), bEndianIndependent(bEndianIndependent), bOwnArchive(bOwnArchive)
{

}

FSerializer::~FSerializer()
{
	if (bOwnArchive)
	{
		delete Archive;
	}
}

void FSerializer::Serialize(void* Data, u32 ByteSize)
{
	if (bEndianIndependent)
	{
		SerializeEndianIndependent(Data, ByteSize);
	}
	else
	{
		SerializeRaw(Data, ByteSize);
	}
}

void FSerializer::SerializeEndianIndependent(void* Data, u32 ByteSize)
{
	if (bIsLoading)
	{
		Archive->Read(Data, ByteSize);
		if (IsBigEndian() == false && ByteSize > 1)
		{
			SwapBytes((char*)Data, ByteSize);
		}
	}
	else
	{
		if (IsBigEndian() == false && ByteSize > 1)
		{
			if (EndiandSwapBuffer.Size() < ByteSize)
			{
				EndiandSwapBuffer.Resize(ByteSize);
			}
			memcpy(EndiandSwapBuffer.Data(), Data, ByteSize);
			SwapBytes(EndiandSwapBuffer.Data(), ByteSize);
			Archive->Write(EndiandSwapBuffer.Data(), ByteSize);
		}
		else
		{
			Archive->Write(Data, ByteSize);
		}
	}
}

void FSerializer::SerializeRaw(void* Data, u32 ByteSize)
{
	if (bIsLoading)
	{
		Archive->Read(Data, ByteSize);
	}
	else
	{
		Archive->Write(Data, ByteSize);
	}
}

void FSerializer::SerializeElements(void* Data, u32 ByteSize, u32 ElementSize)
{
	if (bEndianIndependent)
	{
		SerializeElementsEndianIndependent(Data, ByteSize, ElementSize);
	}
	else
	{
		SerializeElementsRaw(Data, ByteSize, ElementSize);
	}
}

void FSerializer::SerializeElementsEndianIndependent(void* Data, u32 ByteSize, u32 ElementSize)
{
	if (bIsLoading)
	{
		Archive->Read(Data, ByteSize);
		if (IsBigEndian() == false && ByteSize > 1)
		{
			char* DataPtr = (char*)Data;
			u32 ElementCount = ByteSize / ElementSize;
			for (u32 i = 0; i < ElementCount; i++)
			{
				SwapBytes(DataPtr + ElementSize * i, ElementSize);
			}
		}
	}
	else
	{
		if (IsBigEndian() == false && ByteSize > 1)
		{
			if (EndiandSwapBuffer.Size() < ByteSize)
			{
				EndiandSwapBuffer.Resize(ByteSize);
			}

			memcpy(EndiandSwapBuffer.Data(), Data, ByteSize);

			char* DataPtr = (char*)EndiandSwapBuffer.Data();
			u32 ElementCount = ByteSize / ElementSize;
			for (u32 i = 0; i < ElementCount; i++)
			{
				SwapBytes(DataPtr + ElementSize * i, ElementSize);
			}

			Archive->Write(EndiandSwapBuffer.Data(), ByteSize);
		}
		else
		{
			Archive->Write(Data, ByteSize);
		}
	}
}

void FSerializer::SerializeElementsRaw(void* Data, u32 ByteSize, u32 ElementSize)
{
	if (bIsLoading)
	{
		Archive->Read(Data, ByteSize);
	}
	else
	{
		Archive->Write(Data, ByteSize);
	}
}
