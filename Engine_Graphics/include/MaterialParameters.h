#ifndef MaterialParameters_h__
#define MaterialParameters_h__

#include <string>
#include "Vector.h"
#include <d3d11.h>
#include "D3DCompiler.h"
#include <functional>
#include "GraphicsBuffer.h"
#include "UniquePtr.h"

typedef std::function<void(ID3D11DeviceContext*, const TVector<ID3D11Buffer*>&)> SetUniformsCallback;
typedef std::function<void(ID3D11DeviceContext*, const TVector<ID3D11ShaderResourceView*>&)> SetResourceViewsCallback;

struct FShaderResources
{
	TVector<ID3D11Buffer*> UniformsSlots;
	TVector<ID3D11ShaderResourceView*> ResourceViews;
	TVector<std::string> ResourceViewsName;

	void SetCallbacks(SetUniformsCallback UniformsCallback, SetResourceViewsCallback ResourcesCallback)
	{
		this->SetBuffersCallback = UniformsCallback;
		this->SetResourcesCallback = ResourcesCallback;
	}

	bool HasUniforms()
	{
		return UniformsSlots.Size() > 0;
	}

	bool HasResourceViews()
	{
		return ResourceViews.Size() > 0;
	}

	bool HasResource(const std::string& ResourceName)
	{
		for (SizeType i = 0; i < ResourceViewsName.Size(); i++)
		{
			if (ResourceViewsName[i] == ResourceName)
			{
				return true;
			}
		}

		return false;
	}

	bool SetResource(const std::string& ResourceName, ID3D11ShaderResourceView* ResourceView)
	{
		for (SizeType i = 0; i < ResourceViewsName.Size(); i++)
		{
			if (ResourceViewsName[i] == ResourceName)
			{
				ResourceViews[i] = ResourceView;
				return true;
			}
		}

		return false;
	}

	void BindResources(ID3D11DeviceContext* DeviceContext)
	{
		if (UniformsSlots.Size() > 0)
		{
			SetBuffersCallback(DeviceContext, UniformsSlots);
		}
		if (ResourceViews.Size() > 0)
		{
			SetResourcesCallback(DeviceContext, ResourceViews);
		}
	}

private:
	SetUniformsCallback SetBuffersCallback;
	SetResourceViewsCallback SetResourcesCallback;
};

class FMaterialParameters
{
public:
	FMaterialParameters(class FGraphicsContext* GraphicsContext, ID3D11ShaderReflection* VertexShaderReflection, ID3D11ShaderReflection* PixelShaderReflection, ID3D11ShaderReflection* GeometryShaderReflection, ID3D11ShaderReflection* HullShaderReflection, ID3D11ShaderReflection* DomainShaderReflection);
	~FMaterialParameters();

	static void FlushShaderResources(ID3D11DeviceContext* DeviceContext);

	bool UpdateUniform(ID3D11DeviceContext* DeviceContext, const std::string& UniformName, void* Data, int DataSize);
	bool SetResource(const std::string& ResourceName, ID3D11ShaderResourceView* Resource);
	bool SetSampler(const std::string& SamplerName, ID3D11SamplerState* SamplerState);

	bool HasResource(const std::string& ResourceName);
	bool HasUniform(const std::string& UniformName);

	void Apply(ID3D11DeviceContext* DeviceContext);

	struct FUniformBuffer
	{
	public:
		FGraphicsBuffer* GetBufferHandle() const { return ConstantBuffer.Get(); }
		~FUniformBuffer();

	private:
		FUniformBuffer(const std::string& Name, FGraphicsContext* GraphicsContext, int BufferSize);
		FUniformBuffer(const FUniformBuffer& Buffer);
		std::string Name;
		TUniquePtr<FGraphicsBuffer> ConstantBuffer;

		friend class FMaterialParameters;
	};

	FUniformBuffer* GetUniformBuffer(const std::string& Name);
private:

	void AllocateRequiredResources(FGraphicsContext* GraphicsContext, ID3D11ShaderReflection* ShaderReflection, FShaderResources& ShaderResources);
	FUniformBuffer* CreateUniformBuffer(const std::string& Name, FGraphicsContext* GraphicsContext, int BufferSize);

	TVector<TUniquePtr<FUniformBuffer>> Uniforms;
	TVector<std::string> SamplersName;

	TVector<ID3D11SamplerState*> Samplers;

	FShaderResources VertexShaderResources;
	FShaderResources PixelShaderResources;
	FShaderResources GeometryShaderResources;
	FShaderResources HullShaderResources;
	FShaderResources DomainShaderResources;
};

#endif // MaterialParameters_h__
