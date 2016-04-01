#ifndef FileArchive_h__
#define FileArchive_h__

#include "Archive.h"
#include <fstream>
#include <vector>

class FFileArchive : public IArchive
{
public:
	FFileArchive(const std::string& Filename, bool bIsWriting);
	~FFileArchive();

	virtual void Write(const void* Data, u32 ByteSize) override;
	virtual void Read(void* Data, u32 ByteSize) override;

	bool IsSuccessful() const;
	SizeType GetSize() const;
private:
	bool bIsWriting;
	std::fstream CurrentStream;
	TVector<u8> FileBuffer;
	u32 CurrentIndex;
};


#endif // FileArchive_h__
