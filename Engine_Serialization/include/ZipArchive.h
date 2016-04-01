#ifndef CompressedMemoryArchive_h__
#define CompressedMemoryArchive_h__

#include "Archive.h"
#include "zip.h"
#include "unzip.h"

struct FVectorMemoryFile
{
	FVectorMemoryFile(TVector<u8>* Vector = nullptr) : Vector(Vector), CurrentPosition(0) { }
	TVector<u8>* Vector;
	SizeType CurrentPosition;
};

enum class EZipArchiveMode
{
	Write,
	Read,
};

class FZipArchive : public IArchive
{
public:
	class FSubFileArchive : public IArchive
	{
	public:
		virtual void Write(const void* Data, u32 ByteSize) override;
		virtual void Read(void* Data, u32 ByteSize) override;
	private:
		FSubFileArchive(FZipArchive* Archive, const std::string& Name);
		~FSubFileArchive();
		FZipArchive* ParentArchive;
		bool bIsValid;
		friend class FZipArchive;
	};

	FZipArchive(EZipArchiveMode ArchiveMode);
	~FZipArchive();

	bool OpenFile(const std::string& Filename);
	bool OpenMemory(TVector<u8>& OutMemory);

	bool OpenSubFile(const std::string& Name);
	bool CloseSubFile();

	FSubFileArchive* OpenSubFileArchive(const std::string& Name);

	bool Close();

	bool OpenFirstSubFile();
	// These two methods are only meaningful in read mode.
	bool GetCurrentSubFileName(std::string& OutName);
	bool GetCurrentSubFileSize(SizeType OutSize);
	bool OpenNextSubFile();

	virtual void Write(const void* Data, u32 ByteSize) override;
	virtual void Read(void* Data, u32 ByteSize) override;
private:
	zipFile CurrentFile;
	FVectorMemoryFile MemoryFile;
	EZipArchiveMode ArchiveMode;
};

#endif // CompressedMemoryArchive_h__
