#ifndef GraphicsBuffer_h__
#define GraphicsBuffer_h__

#include <d3d11.h>
#include "CoreTypes.h"

enum EBufferType
{
	Vertex = 1,
	Index = 2,
	Uniform = 4,
	UnorderedAccess = 8,
	StreamOutput = 16,
	ShaderResource = 32,
};

enum EBufferUsage
{
	Static,
	Dynamic,
	Staging,
	Default,
};

class FGraphicsBuffer
{
public:
	FGraphicsBuffer(class FGraphicsContext* GraphicsContext, EBufferType Type, EBufferUsage Usage, int SizeInBytes, const void* InitialData, bool bIsStructuredBuffer = false);
	~FGraphicsBuffer();

	void UploadData(ID3D11DeviceContext* DeviceContext, void* Data, int SizeInBytes);
	void* MapData(ID3D11DeviceContext* DeviceContext);
	void UnMapData(ID3D11DeviceContext* DeviceContext);

	void CreateResourceView(class FGraphicsContext* GraphicsContext, DXGI_FORMAT Format, SizeType ElementWidth);
	void CreateUnorderedAccessView(class FGraphicsContext* GraphicsContext, DXGI_FORMAT Format, SizeType NumElements);

	ID3D11Buffer* GetBuffer() const { return Buffer; }
	ID3D11ShaderResourceView* GetResourceView() const { return ResourceView; }
	ID3D11UnorderedAccessView* GetUnorderdAccessView() const { return UnorderedAccessView; }
private:
	ID3D11Buffer* Buffer = nullptr;
	ID3D11ShaderResourceView* ResourceView = nullptr;
	ID3D11UnorderedAccessView* UnorderedAccessView = nullptr;
	bool bIsDynamic = false;
};

#endif // GraphicsBuffer_h__
