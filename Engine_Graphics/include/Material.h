#ifndef Material_h__
#define Material_h__

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <d3d11.h>
#include "D3DCompiler.h"
#include "MaterialParameters.h"
#include "UniquePtr.h"

class FGraphicsContext;

class FShaderProgramInfo
{
public:
	FShaderProgramInfo(const std::wstring& SourcePath, const std::string& EntryPoint, const std::string& ShaderTarget) :
		SourcePath(SourcePath), EntryPoint(EntryPoint), ShaderTarget(ShaderTarget) { }

	FShaderProgramInfo& AddMacroDefine(LPCSTR Name, LPCSTR Value)
	{
		D3D_SHADER_MACRO Define;
		Define.Name = Name;
		Define.Definition = Value;
		ShaderDefines.Add(Define);
		return *this;
	}

	std::string GetDebugName() const;

	bool Compile(ID3DBlob** CodeBlob) const;
private:
	std::wstring SourcePath;
	std::string EntryPoint, ShaderTarget;
	TVector<D3D_SHADER_MACRO> ShaderDefines;
};

class FMaterial
{
public:
	FMaterial(FGraphicsContext* GraphicsContext);
	~FMaterial();

	void Compile(const FShaderProgramInfo* VertexShaderInfo, const FShaderProgramInfo* PixelShaderInfo, const FShaderProgramInfo* GeometryShaderInfo = nullptr, const FShaderProgramInfo* HullShaderInfo = nullptr, const FShaderProgramInfo* DomainShaderInfo = nullptr);

	void Apply(ID3D11DeviceContext* DeviceContext);
	FMaterialParameters* GetParameters()  { return MaterialParameters.Get(); }
private:
	void Release();

	HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3D11ShaderReflection* VertexShaderReflection, ID3DBlob* VertexShaderBlob, ID3D11Device* Device);
	class FGraphicsContext* GraphicsContext;

	TUniquePtr<FMaterialParameters> MaterialParameters;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
};

#endif // Material_h__
