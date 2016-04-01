#include "ZipArchive.h"
#include "ioapi_mem.h"

#define BUFFER_SIZE 1024

static int Max(int A, int B)
{
	return A > B ? A : B;
}

voidpf ZCALLBACK OpenMemoryFunc(voidpf Opaque, const char* Filename, int Mode)
{
	FVectorMemoryFile* Mem = (FVectorMemoryFile*)Opaque;
	if (Mem == NULL)
		return NULL; /* Mem structure passed in was null */

	if (Mode & ZLIB_FILEFUNC_MODE_CREATE)
	{
		Mem->Vector->Resize(BUFFER_SIZE);
	}

	Mem->CurrentPosition = 0;
	return Mem;
}

uLong ZCALLBACK ReadMemoryFunc(voidpf Opaque, voidpf Stream, void* Buf, uLong Size)
{
	FVectorMemoryFile* Mem = (FVectorMemoryFile*)Opaque;
	if (Size > Mem->Vector->Size() - Mem->CurrentPosition)
		Size = Mem->Vector->Size() - Mem->CurrentPosition;

	memcpy(Buf, Mem->Vector->Data() + Mem->CurrentPosition, Size);
	Mem->CurrentPosition += Size;

	return Size;
}

uLong ZCALLBACK WriteMemoryFunc(voidpf Opaque, voidpf Stream, const void* Buf, uLong Size)
{
	FVectorMemoryFile* Mem = (FVectorMemoryFile*)Opaque;

	char *newbase = NULL;
	uLong newmemsize = 0;

	if (Size > Mem->Vector->Size() - Mem->CurrentPosition)
	{
		Mem->Vector->Resize(Mem->Vector->Size() + Max(Size, BUFFER_SIZE));
	}

	memcpy(Mem->Vector->Data() + Mem->CurrentPosition, Buf, Size);
	Mem->CurrentPosition += Size;

	return Size;
}

long ZCALLBACK TellMemoryFunc(voidpf Opaque, voidpf Stream)
{
	FVectorMemoryFile* Mem = (FVectorMemoryFile*)Opaque;
	return Mem->CurrentPosition;
}

long ZCALLBACK SeekMemoryFunc(voidpf Opaque, voidpf Stream, uLong Offset, int Origin)
{
	FVectorMemoryFile* Mem = (FVectorMemoryFile*)Opaque;
	SizeType NewPos;
	switch (Origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:
		NewPos = Mem->CurrentPosition + Offset;
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		NewPos = Mem->Vector->Size() + Offset;
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		NewPos = Offset;
		break;
	default:
		return -1;
	}

	if (NewPos > Mem->Vector->Size())
		return 1; /* Failed to seek that far */

	Mem->CurrentPosition = NewPos;
	return 0;
}

int ZCALLBACK CloseMemoryFunc(voidpf opaque, voidpf stream)
{
	return 0;
}

int ZCALLBACK ErrorMemoryFunc(voidpf Opaque, voidpf Stream)
{
	return 0;
}

voidpf ZCALLBACK OpenDiskFunc(voidpf Opaque, voidpf Stream, int NumberDisk, int Mode)
{
	return nullptr;
}

static void CreateZipFuncDefFromVector(FVectorMemoryFile* VectorMemory, zlib_filefunc_def* FileDef)
{
	FileDef->opaque = VectorMemory;
	FileDef->zopen_file = OpenMemoryFunc;
	FileDef->zclose_file = CloseMemoryFunc;
	FileDef->zread_file = ReadMemoryFunc;
	FileDef->zwrite_file = WriteMemoryFunc;
	FileDef->zseek_file = SeekMemoryFunc;
	FileDef->ztell_file = TellMemoryFunc;
	FileDef->zerror_file = ErrorMemoryFunc;
	FileDef->zopendisk_file = OpenDiskFunc;
}

bool FZipArchive::OpenFile(const std::string& Filename)
{
	if (ArchiveMode == EZipArchiveMode::Write)
	{
		CurrentFile = zipOpen(Filename.c_str(), APPEND_STATUS_CREATE);
	}
	else
	{
		CurrentFile = unzOpen(Filename.c_str());
	}

	return CurrentFile != 0;
}

bool FZipArchive::OpenMemory(TVector<u8>& OutMemory)
{
	MemoryFile.Vector = &OutMemory;
	MemoryFile.CurrentPosition = 0;
	zlib_filefunc_def filefunc32 = { 0 };
	CreateZipFuncDefFromVector(&MemoryFile, &filefunc32);
	if (ArchiveMode == EZipArchiveMode::Write)
	{
		CurrentFile = zipOpen3("__notused__", APPEND_STATUS_CREATE, 0, 0, &filefunc32);
	}
	else
	{
		CurrentFile = unzOpen2("__notused__", &filefunc32);
	}

	return CurrentFile != 0;
}

void FZipArchive::Write(const void* Data, u32 ByteSize)
{
	zipWriteInFileInZip(CurrentFile, Data, ByteSize);
}

void FZipArchive::Read(void* Data, u32 ByteSize)
{
	unzReadCurrentFile(CurrentFile, Data, ByteSize);
}

FZipArchive::FZipArchive(EZipArchiveMode ArchiveMode) : ArchiveMode(ArchiveMode)
{
	CurrentFile = nullptr;
}

FZipArchive::~FZipArchive()
{
	Close();
}

FZipArchive::FSubFileArchive* FZipArchive::OpenSubFileArchive(const std::string& Name)
{
	return new FZipArchive::FSubFileArchive(this, Name);
}

bool FZipArchive::OpenSubFile(const std::string& Name)
{
	if (ArchiveMode == EZipArchiveMode::Write)
	{
		zip_fileinfo file_info = { 0 };
		return zipOpenNewFileInZip(CurrentFile, Name.c_str(), &file_info, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != 0;
	}
	else
	{
		bool bResult = unzLocateFile(CurrentFile, Name.c_str(), 0) == UNZ_OK;
		if (bResult)
		{
			return unzOpenCurrentFile(CurrentFile) == UNZ_OK;
		}
		return bResult;
	}
}

bool FZipArchive::CloseSubFile()
{
	if (ArchiveMode == EZipArchiveMode::Write)
	{
		return zipCloseFileInZip(CurrentFile) == ZIP_OK;
	}
	else
	{
		return unzCloseCurrentFile(CurrentFile) == UNZ_OK;
	}
}

bool FZipArchive::GetCurrentSubFileName(std::string& OutName)
{
	if (ArchiveMode == EZipArchiveMode::Read)
	{
		OutName.resize(128);
		unz_file_info FileInfo;
		int Result = unzGetCurrentFileInfo(CurrentFile, &FileInfo, (char*)OutName.data(), OutName.size(), nullptr, 0, nullptr, 0);
		OutName.resize(strlen(OutName.data()));
		return Result == UNZ_OK;
	}
	return false;
}

bool FZipArchive::GetCurrentSubFileSize(SizeType OutSize)
{
	OutSize = 0;
	if (ArchiveMode == EZipArchiveMode::Read)
	{
		unz_file_info FileInfo;
		int Result = unzGetCurrentFileInfo(CurrentFile, &FileInfo, nullptr, 0, nullptr, 0, nullptr, 0);
		OutSize = FileInfo.uncompressed_size;
		return Result == UNZ_OK;
	}
	return false;
}

bool FZipArchive::OpenFirstSubFile()
{
	if (ArchiveMode == EZipArchiveMode::Read)
	{
		int Result = unzGoToFirstFile(CurrentFile);
		if (Result == UNZ_OK)
		{
			return unzOpenCurrentFile(CurrentFile) == UNZ_OK;
		}
	}
	return false;
}

bool FZipArchive::OpenNextSubFile()
{
	if (ArchiveMode == EZipArchiveMode::Read)
	{
		unzCloseCurrentFile(CurrentFile);
		int Result = unzGoToNextFile(CurrentFile);
		if (Result == UNZ_OK)
		{
			return unzOpenCurrentFile(CurrentFile) == UNZ_OK;
		}
		else
			return false;
	}
	return false;
}

bool FZipArchive::Close()
{
	bool bResult = false;

	if (CurrentFile != nullptr)
	{
		if (ArchiveMode == EZipArchiveMode::Write)
		{
			zipCloseFileInZip(CurrentFile);
			bResult = zipClose(CurrentFile, nullptr) == ZIP_OK;
		}
		else
		{
			unzCloseCurrentFile(CurrentFile);
			bResult = unzClose(CurrentFile) == UNZ_OK;
		}
	}

	CurrentFile = nullptr;
	return bResult;
}

void FZipArchive::FSubFileArchive::Write(const void* Data, u32 ByteSize)
{
	ParentArchive->Write(Data, ByteSize);
}

void FZipArchive::FSubFileArchive::Read(void* Data, u32 ByteSize)
{
	ParentArchive->Read(Data, ByteSize);
}

FZipArchive::FSubFileArchive::FSubFileArchive(FZipArchive* Archive, const std::string& Name) : ParentArchive(Archive)
{
	bIsValid = Archive->OpenSubFile(Name);
}

FZipArchive::FSubFileArchive::~FSubFileArchive()
{
	ParentArchive->CloseSubFile();
}
