#include "ComputeShader.h"
#include "GraphicsContext.h"
#include "BlobPointer.h"
#include "Material.h"

FComputeShader::FComputeShader(FGraphicsContext* GraphicsContext, const FShaderProgramInfo* ComputeShaderInfo)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	FBlobPointer ShaderBlob;

	if (ComputeShaderInfo->Compile(ShaderBlob.GetReference()))
	{
		Device->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &ComputeShader);
	}
}

FComputeShader::~FComputeShader()
{
	if (ComputeShader != nullptr)
	{
		ComputeShader->Release();
	}
}

void FComputeShader::Execute(ID3D11DeviceContext* DeviceContext, int ThreadGroupCountX, int ThreadGroupCountY, int ThreadGroupCountZ)
{
	DeviceContext->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void FComputeShader::Bind(ID3D11DeviceContext* DeviceContext)
{
	DeviceContext->CSSetShader(ComputeShader, nullptr, 0);
}

void FComputeShader::SetShaderResources(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView** Resources, int ResourceCount)
{
	DeviceContext->CSSetShaderResources(0, ResourceCount, Resources);
}

void FComputeShader::SetShaderUnorderedAccessViews(ID3D11DeviceContext* DeviceContext, ID3D11UnorderedAccessView** Uavs, int UavsCount, const UINT* InitialCounts)
{
	DeviceContext->CSSetUnorderedAccessViews(0, UavsCount, Uavs, InitialCounts);
}

void FComputeShader::SetShaderConstantBuffers(ID3D11DeviceContext* DeviceContext, ID3D11Buffer** Buffers, int BuffersCount)
{
	DeviceContext->CSSetConstantBuffers(0, BuffersCount, Buffers);
}
