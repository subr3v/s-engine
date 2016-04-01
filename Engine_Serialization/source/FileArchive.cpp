#include "FileArchive.h"

FFileArchive::FFileArchive(const std::string& Filename, bool bIsWriting) : bIsWriting(bIsWriting)
{
	if (bIsWriting)
	{
		CurrentStream = std::fstream(Filename, std::ios::out | std::ios::binary);
	}
	else
	{
		CurrentStream = std::fstream(Filename, std::ios::in | std::ios::binary);

		std::streamoff FileSize = CurrentStream.tellg();
		CurrentStream.seekg(0, std::ios::end);
		FileSize = (u32)CurrentStream.tellg() - FileSize;
		CurrentStream.seekg(0, std::ios::beg);
		FileBuffer.Resize((SizeType)FileSize);

		if (FileSize > 0)
		{
			CurrentStream.read((char*)FileBuffer.Data(), FileSize);
			CurrentIndex = 0;
		}
	}
}

FFileArchive::~FFileArchive()
{

}

void FFileArchive::Write(const void* Data, u32 ByteSize)
{
	if (bIsWriting)
		CurrentStream.write((const char*)Data, ByteSize);
}

void FFileArchive::Read(void* Data, u32 ByteSize)
{
	if (bIsWriting == false && ByteSize > 0)
	{
		memcpy(Data, FileBuffer.Data() + CurrentIndex, ByteSize);
		CurrentIndex += ByteSize;
		//CurrentStream.read((char*)Data, ByteSize);
	}
}

bool FFileArchive::IsSuccessful() const
{
	return CurrentStream.is_open();
}

SizeType FFileArchive::GetSize() const
{
	return FileBuffer.Size();
}
