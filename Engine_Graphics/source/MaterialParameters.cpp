#include "MaterialParameters.h"
#include "GraphicsContext.h"
#include "GraphicsBuffer.h"
#include "Profiler.h"

#define CONCAT_NAME(a, b) a##b

#define DefResourceCallbacks(ShaderType) \
	static void CONCAT_NAME(UpdateBuffers, ShaderType)(ID3D11DeviceContext* DeviceContext, const TVector<ID3D11Buffer*>& Buffers) \
	{ \
		DeviceContext->CONCAT_NAME(ShaderType, SetConstantBuffers)(0, Buffers.Size(), Buffers.Data()); \
	} \
	static void CONCAT_NAME(UpdateResources, ShaderType)(ID3D11DeviceContext* DeviceContext, const TVector<ID3D11ShaderResourceView*>& Resources) \
	{ \
		DeviceContext->CONCAT_NAME(ShaderType, SetShaderResources)(0, Resources.Size(), Resources.Data()); \
	}															

DefResourceCallbacks(VS)
DefResourceCallbacks(PS)
DefResourceCallbacks(GS)
DefResourceCallbacks(DS)
DefResourceCallbacks(HS)

FMaterialParameters::FMaterialParameters(FGraphicsContext* GraphicsContext, ID3D11ShaderReflection* VertexShaderReflection, ID3D11ShaderReflection* PixelShaderReflection, ID3D11ShaderReflection* GeometryShaderReflection, ID3D11ShaderReflection* HullShaderReflection, ID3D11ShaderReflection* DomainShaderReflection)
{
	// Initialise callbacks
	VertexShaderResources.SetCallbacks(&UpdateBuffersVS, &UpdateResourcesVS);
	PixelShaderResources.SetCallbacks(&UpdateBuffersPS, &UpdateResourcesPS);
	GeometryShaderResources.SetCallbacks(&UpdateBuffersGS, &UpdateResourcesGS);
	HullShaderResources.SetCallbacks(&UpdateBuffersHS, &UpdateResourcesHS);
	DomainShaderResources.SetCallbacks(&UpdateBuffersDS, &UpdateResourcesDS);

	AllocateRequiredResources(GraphicsContext, VertexShaderReflection, VertexShaderResources);
	AllocateRequiredResources(GraphicsContext, PixelShaderReflection, PixelShaderResources);

	if (GeometryShaderReflection != nullptr)
		AllocateRequiredResources(GraphicsContext, GeometryShaderReflection, GeometryShaderResources);

	if (HullShaderReflection != nullptr)
		AllocateRequiredResources(GraphicsContext, HullShaderReflection, HullShaderResources);

	if (DomainShaderReflection != nullptr)
		AllocateRequiredResources(GraphicsContext, DomainShaderReflection, DomainShaderResources);
}

FMaterialParameters::~FMaterialParameters()
{
	Uniforms.Clear();
}

void FMaterialParameters::AllocateRequiredResources(FGraphicsContext* GraphicsContext, ID3D11ShaderReflection* ShaderReflection, FShaderResources& ShaderResources)
{
	D3D11_SHADER_DESC ShaderDesc;
	ShaderReflection->GetDesc(&ShaderDesc);

	ShaderResources.UniformsSlots.Resize(ShaderDesc.ConstantBuffers);

	for (size_t i = 0; i < ShaderDesc.ConstantBuffers; i++)
	{
		auto BufferReflection = ShaderReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC Desc;
		BufferReflection->GetDesc(&Desc);

		FUniformBuffer* Buffer = GetUniformBuffer(Desc.Name);
		if (Buffer == nullptr)
		{
			Buffer = CreateUniformBuffer(Desc.Name, GraphicsContext, Desc.Size);
		}

		ShaderResources.UniformsSlots[i] = Buffer->ConstantBuffer->GetBuffer();
	}

	for (size_t i = 0; i < ShaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC Desc;
		ShaderReflection->GetResourceBindingDesc(i, &Desc);
		
		if (Desc.Type == D3D_SIT_SAMPLER)
		{
			Samplers.Add(nullptr);
			SamplersName.Add(Desc.Name);
		}
		else if (Desc.Type == D3D_SIT_TEXTURE)
		{
			ShaderResources.ResourceViews.Add(nullptr);
			ShaderResources.ResourceViewsName.Add(Desc.Name);
		}
	}
}

void FMaterialParameters::FlushShaderResources(ID3D11DeviceContext* DeviceContext)
{
	ID3D11ShaderResourceView* Resources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };

	DeviceContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, Resources);
	DeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, Resources);
	DeviceContext->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, Resources);
	DeviceContext->HSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, Resources);
	DeviceContext->DSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, Resources);
}

void FMaterialParameters::Apply(ID3D11DeviceContext* DeviceContext)
{
	if (Samplers.Size() > 0)
	{
		DeviceContext->VSSetSamplers(0, Samplers.Size(), Samplers.Data());
		DeviceContext->PSSetSamplers(0, Samplers.Size(), Samplers.Data());

		// Do these only if shaders are enabled.
		DeviceContext->HSSetSamplers(0, Samplers.Size(), Samplers.Data());
		DeviceContext->DSSetSamplers(0, Samplers.Size(), Samplers.Data());
		DeviceContext->GSSetSamplers(0, Samplers.Size(), Samplers.Data());
	}

	VertexShaderResources.BindResources(DeviceContext);
	PixelShaderResources.BindResources(DeviceContext);
	GeometryShaderResources.BindResources(DeviceContext);
	HullShaderResources.BindResources(DeviceContext);
	DomainShaderResources.BindResources(DeviceContext);
}

FMaterialParameters::FUniformBuffer* FMaterialParameters::CreateUniformBuffer(const std::string& Name, FGraphicsContext* GraphicsContext, int BufferSize)
{
	auto Buffer = new FUniformBuffer(Name, GraphicsContext, BufferSize);
	Uniforms.Add(Buffer);
	return Buffer;
}

FMaterialParameters::FUniformBuffer* FMaterialParameters::GetUniformBuffer(const std::string& Name)
{
	for (auto& UniformBuffer : Uniforms)
	{
		if (UniformBuffer->Name == Name)
			return UniformBuffer.Get();
	}

	return nullptr;
}

bool FMaterialParameters::UpdateUniform(ID3D11DeviceContext* DeviceContext, const std::string& UniformName, void* Data, int DataSize)
{
	FUniformBuffer* Buffer = GetUniformBuffer(UniformName);
	if (Buffer != nullptr)
	{
		Buffer->ConstantBuffer->UploadData(DeviceContext, Data, DataSize);
		return true;
	}
	//OutputDebugString("WARNING: Attempt to update a uniform that doesn't exist!");
	return false;
}

bool FMaterialParameters::SetResource(const std::string& ResourceName, ID3D11ShaderResourceView* Resource)
{
	FProfiler::Get().ProfileCounter("Rendering", "Set Shader Resource", true, 1);

	bool bSuccess = VertexShaderResources.SetResource(ResourceName, Resource);
	bSuccess |= PixelShaderResources.SetResource(ResourceName, Resource);
	bSuccess |= DomainShaderResources.SetResource(ResourceName, Resource);
	bSuccess |= HullShaderResources.SetResource(ResourceName, Resource);
	bSuccess |= GeometryShaderResources.SetResource(ResourceName, Resource);

	return bSuccess;
}

bool FMaterialParameters::SetSampler(const std::string& Name, ID3D11SamplerState* SamplerState)
{
	for (size_t i = 0; i < SamplersName.Size(); i++)
	{
		if (SamplersName[i] == Name)
		{
			Samplers[i] = SamplerState;
			return true;
		}
	}

	return false;
}

bool FMaterialParameters::HasResource(const std::string& ResourceName)
{
	bool bSuccess = VertexShaderResources.HasResource(ResourceName);
	bSuccess |= PixelShaderResources.HasResource(ResourceName);
	bSuccess |= DomainShaderResources.HasResource(ResourceName);
	bSuccess |= HullShaderResources.HasResource(ResourceName);
	bSuccess |= GeometryShaderResources.HasResource(ResourceName);

	return bSuccess;
}

bool FMaterialParameters::HasUniform(const std::string& UniformName)
{
	return GetUniformBuffer(UniformName) != nullptr;
}

FMaterialParameters::FUniformBuffer::FUniformBuffer(const std::string& Name, FGraphicsContext* GraphicsContext, int BufferSize) : Name(Name)
{
	ConstantBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, EBufferType::Uniform, EBufferUsage::Dynamic, BufferSize, nullptr);
}

FMaterialParameters::FUniformBuffer::~FUniformBuffer()
{

}