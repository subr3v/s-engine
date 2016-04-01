#include "GraphicsBuffer.h"
#include "GraphicsContext.h"
#include <cassert>
#include "Profiler.h"

static int GetBindingFlags(EBufferType BufferType)
{
	int Result = 0;
	Result |= (BufferType & EBufferType::Uniform) ? D3D11_BIND_CONSTANT_BUFFER : 0;
	Result |= (BufferType & EBufferType::Vertex) ? D3D11_BIND_VERTEX_BUFFER : 0;
	Result |= (BufferType & EBufferType::Index) ? D3D11_BIND_INDEX_BUFFER : 0;
	Result |= (BufferType & EBufferType::UnorderedAccess) ? D3D11_BIND_UNORDERED_ACCESS : 0;
	Result |= (BufferType & EBufferType::StreamOutput) ? D3D11_BIND_STREAM_OUTPUT : 0;
	Result |= (BufferType & EBufferType::ShaderResource) ? D3D11_BIND_SHADER_RESOURCE : 0;
	return Result;
}

static D3D11_USAGE GetUsageFlags(EBufferUsage Usage)
{
	switch (Usage)
	{
	case EBufferUsage::Static:
		return D3D11_USAGE_IMMUTABLE;
	case EBufferUsage::Dynamic:
		return D3D11_USAGE_DYNAMIC;
	case EBufferUsage::Staging:
		return D3D11_USAGE_STAGING;
	case EBufferUsage::Default:
		return D3D11_USAGE_DEFAULT;
	}

	return D3D11_USAGE_DEFAULT;
}

static int GetCpuFlags(EBufferType Type, EBufferUsage Usage)
{
	if (Usage == EBufferUsage::Dynamic || Usage == EBufferUsage::Staging)
		return D3D11_CPU_ACCESS_WRITE;

	return 0;
}

FGraphicsBuffer::FGraphicsBuffer(class FGraphicsContext* GraphicsContext, EBufferType Type, EBufferUsage Usage, int SizeInBytes, const void* InitialData, bool bIsStructuredBuffer)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	D3D11_BUFFER_DESC BufferDescription;
	BufferDescription.ByteWidth = SizeInBytes;
	BufferDescription.Usage = GetUsageFlags(Usage);
	BufferDescription.CPUAccessFlags = GetCpuFlags(Type, Usage);
	BufferDescription.MiscFlags = bIsStructuredBuffer ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	BufferDescription.StructureByteStride = 0;
	BufferDescription.BindFlags = GetBindingFlags(Type);

	if (InitialData != nullptr)
	{
		D3D11_SUBRESOURCE_DATA InitialBufferData;
		InitialBufferData.pSysMem = InitialData;
		InitialBufferData.SysMemPitch = InitialBufferData.SysMemSlicePitch = 0;

		Device->CreateBuffer(&BufferDescription, &InitialBufferData, &Buffer);
	}
	else
	{
		Device->CreateBuffer(&BufferDescription, nullptr, &Buffer);
	}

	assert(Buffer);

	bIsDynamic = (BufferDescription.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) != 0;
}

static void CopyBufferData(void* Destination, void* Source, s32 SizeInBytes)
{
	// We know SizeInBytes is a multiple of 16 here.
	s32* Dst = (s32*)Destination;
	s32* Src = (s32*)Source;
	SizeInBytes /= 4;
	for (s32 i = 0; i < SizeInBytes; i += 4)
	{
		*(Dst + 0) = *(Src + 0);
		*(Dst + 1) = *(Src + 1);
		*(Dst + 2) = *(Src + 2);
		*(Dst + 3) = *(Src + 3);
		Dst += 4;
		Src += 4;
	}
}

void FGraphicsBuffer::UploadData(ID3D11DeviceContext* DeviceContext, void* Data, int SizeInBytes)
{
	assert(bIsDynamic);
	FProfiler::Get().ProfileCounter("Rendering", "Buffer Upload", true, 1);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource) == S_OK)
	{
		if (SizeInBytes % 16 == 0)
			CopyBufferData(MappedResource.pData, Data, SizeInBytes);
		else
			memcpy(MappedResource.pData, Data, SizeInBytes);
	}
	DeviceContext->Unmap(Buffer, 0);
}

FGraphicsBuffer::~FGraphicsBuffer()
{
	if (Buffer != nullptr)
	{
		Buffer->Release();
		Buffer = nullptr;
	}
}

void* FGraphicsBuffer::MapData(ID3D11DeviceContext* DeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource) == S_OK)
	{
		return MappedResource.pData;
	}
	return nullptr;
}

void FGraphicsBuffer::UnMapData(ID3D11DeviceContext* DeviceContext)
{
	DeviceContext->Unmap(Buffer, 0);
}

void FGraphicsBuffer::CreateResourceView(class FGraphicsContext* GraphicsContext, DXGI_FORMAT Format, SizeType ElementWidth)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceViewDescription;
	memset(&ResourceViewDescription, 0, sizeof(ResourceViewDescription));
	ResourceViewDescription.Format = Format;
	ResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	ResourceViewDescription.Buffer.ElementOffset = 0;
	ResourceViewDescription.Buffer.ElementWidth = ElementWidth;

	Device->CreateShaderResourceView(Buffer, &ResourceViewDescription, &ResourceView);
}

void FGraphicsBuffer::CreateUnorderedAccessView(class FGraphicsContext* GraphicsContext, DXGI_FORMAT Format, SizeType NumElements)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	D3D11_UNORDERED_ACCESS_VIEW_DESC UavDescription;
	memset(&UavDescription, 0, sizeof(UavDescription));
	UavDescription.Format = Format;
	UavDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UavDescription.Buffer.NumElements = NumElements;

	Device->CreateUnorderedAccessView(Buffer, &UavDescription, &UnorderedAccessView);
}
