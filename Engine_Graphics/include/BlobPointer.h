#ifndef BlobPointer_h__
#define BlobPointer_h__

struct FBlobPointer
{
public:
	FBlobPointer() { }
	~FBlobPointer()
	{
		if (Blob != nullptr)
		{
			Blob->Release();
			Blob = nullptr;
		}
	}

	ID3D10Blob** GetReference()
	{
		return &Blob;
	}

	operator ID3D10Blob*() const { return Blob; }

	void Set(ID3D10Blob* Blob)
	{
		this->Blob = Blob;
	}

	ID3D10Blob* operator->()
	{
		return Blob;
	}

private:
	ID3D10Blob* Blob = nullptr;
};

#endif // BlobPointer_h__
