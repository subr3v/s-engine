#ifndef ComputeShader_h__
#define ComputeShader_h__

#include <d3d11.h>

class FGraphicsContext;
class FShaderProgramInfo;

class FComputeShader
{
public:
	FComputeShader(FGraphicsContext* GraphicsContext, const FShaderProgramInfo* ComputeShaderInfo);
	~FComputeShader();

	void Bind(ID3D11DeviceContext* DeviceContext);

	void SetShaderResources(ID3D11DeviceContext* DeviceContext, ID3D11ShaderResourceView** Resources, int ResourceCount);
	void SetShaderUnorderedAccessViews(ID3D11DeviceContext* DeviceContext, ID3D11UnorderedAccessView** Uavs, int UavsCount, const UINT* InitialCounts);
	void SetShaderConstantBuffers(ID3D11DeviceContext* DeviceContext, ID3D11Buffer** Buffers, int BuffersCount);

	void Execute(ID3D11DeviceContext* DeviceContext, int ThreadGroupCountX, int ThreadGroupCountY, int ThreadGroupCountZ);

private:
	ID3D11ComputeShader* ComputeShader = nullptr;

};

#endif // ComputeShader_h__
