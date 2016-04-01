#include "Material.h"
#include "Texture.h"
#include "GraphicsContext.h"
#include <locale>
#include "BlobPointer.h"

FMaterial::FMaterial(FGraphicsContext* GraphicsContext) : GraphicsContext(GraphicsContext)
{

}

FMaterial::~FMaterial()
{
	GraphicsContext = nullptr;
	Release();
}

void FMaterial::Release()
{
	if (VertexShader != nullptr)
		VertexShader->Release();

	if (PixelShader != nullptr)
		PixelShader->Release();

	if (InputLayout != nullptr)
		InputLayout->Release();

	if (HullShader != nullptr)
		HullShader->Release();

	if (DomainShader != nullptr)
		DomainShader->Release();

	if (GeometryShader != nullptr)
		GeometryShader->Release();
}


void FMaterial::Compile(const FShaderProgramInfo* VertexShaderInfo, const FShaderProgramInfo* PixelShaderInfo, const FShaderProgramInfo* GeometryShaderInfo, const FShaderProgramInfo* HullShaderInfo, const FShaderProgramInfo* DomainShaderInfo)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	// Core shaders blobs
	FBlobPointer VertexShaderBlob, PixelShaderBlob;

	// Optional shaders blobs
	FBlobPointer GeometryShaderBlob, HullShaderBlob, DomainShaderBlob;

	bool bCompilationSuccessful = VertexShaderInfo->Compile(VertexShaderBlob.GetReference()) && PixelShaderInfo->Compile(PixelShaderBlob.GetReference());

	if (GeometryShaderInfo != nullptr)
	{
		GeometryShaderInfo->Compile(GeometryShaderBlob.GetReference());
	}

	if (DomainShaderInfo != nullptr && HullShaderInfo != nullptr)
	{
		DomainShaderInfo->Compile(DomainShaderBlob.GetReference());
		HullShaderInfo->Compile(HullShaderBlob.GetReference());
	}

	if (bCompilationSuccessful)
	{
		Release(); // We release only if compilation was successful. (this makes hot reloading fault tolerant case because we can keep the older shaders running)

		ID3D11ShaderReflection* VertexShaderReflection, *PixelShaderReflection;
		ID3D11ShaderReflection* GeometryShaderReflection = nullptr, *HullShaderReflection = nullptr, *DomainShaderReflection = nullptr;

		Device->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), nullptr, &VertexShader);
		Device->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), nullptr, &PixelShader);

		D3DSetDebugName(VertexShader, VertexShaderInfo->GetDebugName());
		D3DSetDebugName(PixelShader, PixelShaderInfo->GetDebugName());

		D3DReflect(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&VertexShaderReflection);
		D3DReflect(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&PixelShaderReflection);

		if (GeometryShaderBlob)
		{
			Device->CreateGeometryShader(GeometryShaderBlob->GetBufferPointer(), GeometryShaderBlob->GetBufferSize(), nullptr, &GeometryShader);
			D3DSetDebugName(GeometryShader, GeometryShaderInfo->GetDebugName());
			D3DReflect(GeometryShaderBlob->GetBufferPointer(), GeometryShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&GeometryShaderReflection);
		}

		if (DomainShaderBlob && HullShaderBlob)
		{
			Device->CreateHullShader(HullShaderBlob->GetBufferPointer(), HullShaderBlob->GetBufferSize(), nullptr, &HullShader);
			Device->CreateDomainShader(DomainShaderBlob->GetBufferPointer(), DomainShaderBlob->GetBufferSize(), nullptr, &DomainShader);
			
			D3DSetDebugName(HullShader, HullShaderInfo->GetDebugName());
			D3DSetDebugName(DomainShader, DomainShaderInfo->GetDebugName());

			D3DReflect(HullShaderBlob->GetBufferPointer(), HullShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&HullShaderReflection);
			D3DReflect(DomainShaderBlob->GetBufferPointer(), DomainShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&DomainShaderReflection);
		}

		CreateInputLayoutDescFromVertexShaderSignature(VertexShaderReflection, VertexShaderBlob, Device);

		MaterialParameters = Make_Unique<FMaterialParameters>(GraphicsContext, VertexShaderReflection, PixelShaderReflection, GeometryShaderReflection, HullShaderReflection, DomainShaderReflection);

		VertexShaderReflection->Release();
		PixelShaderReflection->Release();

		if (GeometryShaderReflection != nullptr)
			GeometryShaderReflection->Release();

		if (DomainShaderReflection != nullptr)
			DomainShaderReflection->Release();

		if (HullShaderReflection != nullptr)
			HullShaderReflection->Release();
	}
}

void FMaterial::Apply(ID3D11DeviceContext* DeviceContext)
{
	if (VertexShader == nullptr || PixelShader == nullptr || !MaterialParameters)
	{
		//TODO: Add log message here
		return;
	}

	DeviceContext->IASetInputLayout(InputLayout);
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);
	DeviceContext->GSSetShader(GeometryShader, nullptr, 0);
	DeviceContext->HSSetShader(HullShader, nullptr, 0);
	DeviceContext->DSSetShader(DomainShader, nullptr, 0);
	MaterialParameters->Apply(DeviceContext);
}

HRESULT FMaterial::CreateInputLayoutDescFromVertexShaderSignature(ID3D11ShaderReflection* VertexShaderReflection, ID3DBlob* VertexShaderBlob, ID3D11Device* Device)
{
	// Get shader info
	D3D11_SHADER_DESC ShaderDescription;
	VertexShaderReflection->GetDesc(&ShaderDescription);

	// Read input layout description from shader info
	TVector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;
	for (size_t i = 0; i < ShaderDescription.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC ParamDescription;
		VertexShaderReflection->GetInputParameterDesc(i, &ParamDescription);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC ElementDesc;
		ElementDesc.SemanticName = ParamDescription.SemanticName;
		ElementDesc.SemanticIndex = ParamDescription.SemanticIndex;
		ElementDesc.InputSlot = 0;
		ElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ElementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (ParamDescription.Mask == 1)
		{
			if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (ParamDescription.Mask <= 3)
		{
			if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (ParamDescription.Mask <= 7)
		{
			if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (ParamDescription.Mask <= 15)
		{
			if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (ParamDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		if (strcmp(ParamDescription.SemanticName, "COLOR") == 0)
		{
			// Special case: colour is actually a r8b8g8a8_unorm.
			ElementDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		//save element desc
		InputLayoutDesc.Add(ElementDesc);
	}

	// Special case here:
	// If we're using SV_VertexID only then we don't actually need an input layout.

	if (InputLayoutDesc.Size() == 1 && strcmp(InputLayoutDesc[0].SemanticName, "SV_VertexID") != -1)
	{
		return S_OK;
	}

	// Try to create Input Layout
	HRESULT hr = Device->CreateInputLayout(&InputLayoutDesc[0], InputLayoutDesc.Size(), VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout);

	return hr;

}

bool FShaderProgramInfo::Compile(ID3DBlob** CodeBlob) const
{
	TVector<D3D_SHADER_MACRO> NullTerminatedDefines;
	NullTerminatedDefines.Resize(ShaderDefines.Size() + 1);
	for (size_t i = 0; i < ShaderDefines.Size(); i++)
	{
		NullTerminatedDefines[i] = ShaderDefines[i];
	}
	size_t LastIndex = NullTerminatedDefines.Size() - 1;
	NullTerminatedDefines[LastIndex].Name = NullTerminatedDefines[LastIndex].Definition = nullptr;

	ID3DBlob* ErrorBlob = nullptr;

	HRESULT Result = D3DCompileFromFile(SourcePath.c_str(), NullTerminatedDefines.Data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
		EntryPoint.c_str(), ShaderTarget.c_str(), 0, 0, CodeBlob, &ErrorBlob);

	if (Result != S_OK)
	{
		OutputDebugString("ERROR COMPILING SHADER: ");
		if (ErrorBlob != nullptr)
		{
			OutputDebugString((LPCSTR)ErrorBlob->GetBufferPointer());
			ErrorBlob->Release();
		}
	}

	return Result == S_OK;
}

std::string FShaderProgramInfo::GetDebugName() const
{
	//setup converter
	typedef std::codecvt<wchar_t, char, _Mbstatet> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;
	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string SourcePathStr = converter.to_bytes(SourcePath);
	return SourcePathStr + std::string(" - ") + EntryPoint + std::string(" - ") + ShaderTarget;
}
