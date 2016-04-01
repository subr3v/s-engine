#include "MaterialManager.h"
#include "ConstantBufferTypes.h"
#include "GraphicsContext.h"
#include "Material.h"
#include "RenderState.h"
#include "glm/gtc/matrix_transform.hpp"
#include "BaseScene.h"
#include "Camera.h"
#include "ConfigVariable.h"
#include "Profiler.h"

static FConfigVariable MaterialConfigurationPath("Engine", "MaterialConfigPath", "Materials.config");

#define alignas(size) __declspec( align(size) )

#pragma pack(push,4)

// Shader Buffers definition:
struct alignas(16) FFrameDataBuffer
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 InverseViewMatrix;
	glm::mat4 InverseProjectionMatrix;
	glm::mat4 InverseTransposeViewMatrix;
	glm::mat4 SpriteProjectionMatrix;
	glm::vec3 CameraPosition; u32 NumLights; // 0xFFFF0000 has point light num encoded and 0x0000FFFF has spot light num encoded
	float FrameTime; float DeltaTime; glm::vec2 Padding;
	glm::vec2 ScreenSize; glm::vec2 PD2;

};

#pragma pack(pop) 

ConstantString FMaterialManager::kFrameDataBufferName = "FrameBuffer";

FMaterialManager::FMaterialManager(class FGraphicsContext* GraphicsContext) : GraphicsContext(GraphicsContext)
{

}

FMaterialManager::~FMaterialManager()
{

}

void FMaterialManager::CompileMaterial(FMaterial* Material, const FMaterialDescriptor& Descriptor)
{
	FShaderProgramInfo VertexInfo(Descriptor.Path, "Vertex", "vs_5_0");
	FShaderProgramInfo PixelInfo(Descriptor.Path, "Pixel", "ps_5_0");
	FShaderProgramInfo GeometryInfo(Descriptor.Path, "Geometry", "gs_5_0");
	FShaderProgramInfo HullInfo(Descriptor.Path, "Hull", "hs_5_0");
	FShaderProgramInfo DomainInfo(Descriptor.Path, "Domain", "ds_5_0");

	TVector<FShaderProgramInfo*> ShaderInfos = { &VertexInfo, &PixelInfo, nullptr, nullptr, nullptr };

	if (std::find(Descriptor.Defines.begin(), Descriptor.Defines.end(), "GEOMETRY_SHADER") != Descriptor.Defines.end())
	{
		ShaderInfos[2] = &GeometryInfo;
	}

	if (std::find(Descriptor.Defines.begin(), Descriptor.Defines.end(), "TESSELLATION_SHADER") != Descriptor.Defines.end())
	{
		ShaderInfos[3] = &HullInfo;
		ShaderInfos[4] = &DomainInfo;
	}

	for (auto Info : ShaderInfos)
	{
		if (Info == nullptr)
			continue;

		for (auto& Define : Descriptor.Defines)
		{
			Info->AddMacroDefine(Define.c_str(), "1");
		}

		for (auto& Define : GlobalDefines)
		{
			Info->AddMacroDefine(Define.c_str(), "1");
		}
	}

	Material->Compile(ShaderInfos[0], ShaderInfos[1], ShaderInfos[2], ShaderInfos[3], ShaderInfos[4]);
	Material->GetParameters()->SetSampler("TextureSampler", GraphicsContext->GetRenderStates()->TextureSampler);
	Material->GetParameters()->SetSampler("SimpleSampler", GraphicsContext->GetRenderStates()->SimpleSampler);
	Material->GetParameters()->SetSampler("LinearSampler", GraphicsContext->GetRenderStates()->LinearSampler);
}

FMaterial* FMaterialManager::CreateMaterial(FGraphicsContext* GraphicsContext, const FMaterialDescriptor& MaterialDescriptor)
{
	FProfiler::Get().ProfileCounter("Rendering", "Material Count", false, 1);
	FMaterial* Material = new FMaterial(GraphicsContext);
	CompileMaterial(Material, MaterialDescriptor);
	Materials.AddResource(MaterialDescriptor.Name, Material);
	MaterialDescriptors.Add(MaterialDescriptor);
	return Material;
}

void FMaterialManager::LoadMaterials(const TVector<std::string>& GlobalDefines)
{
	this->GlobalDefines = GlobalDefines;

	FMaterialConfiguration Configuration;
	Configuration.LoadFromFile(MaterialConfigurationPath.AsString());

	for (auto Descriptor : Configuration.GetDescriptors())
	{
		CreateMaterial(GraphicsContext, Descriptor);
	}
}

void FMaterialManager::UpdateMaterialBuffers(const FRenderFrameData& FrameData, class FGraphicsContext* GraphicsContext)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	FFrameDataBuffer FrameDataBuffer;

	FrameDataBuffer.FrameTime = FrameData.FrameTime;
	FrameDataBuffer.DeltaTime = FrameData.DeltaTime;
	FrameDataBuffer.ViewMatrix = FrameData.ViewMatrix;
	FrameDataBuffer.InverseViewMatrix = glm::inverse(FrameData.ViewMatrix);
	FrameDataBuffer.ProjectionMatrix = FrameData.ProjectionMatrix;
	FrameDataBuffer.InverseProjectionMatrix = glm::inverse(FrameData.ProjectionMatrix);
	FrameDataBuffer.InverseTransposeViewMatrix = glm::transpose(glm::inverse(FrameData.ViewMatrix));
	FrameDataBuffer.ScreenSize.x = (float)GraphicsContext->GetWidth();
	FrameDataBuffer.ScreenSize.y = (float)GraphicsContext->GetHeight();
	FrameDataBuffer.CameraPosition = glm::vec3(FrameData.ViewMatrix * glm::vec4(FrameData.CameraPosition, 1.0f));
	FrameDataBuffer.NumLights = (FrameData.PointLightCount & 0xFFFF) | ((FrameData.SpotLightCount & 0xFFFF) << 16);

	FProfiler::Get().ProfileCounter("Rendering", "Point Light Count", true, FrameData.PointLightCount);
	FProfiler::Get().ProfileCounter("Rendering", "Spot Light Count", true, FrameData.SpotLightCount);

	if (FrameData.SpriteViewport == nullptr)
	{
		FrameDataBuffer.SpriteProjectionMatrix = glm::ortho(0.0f, (float)GraphicsContext->GetWidth(), (float)GraphicsContext->GetHeight(), 0.0f);
	}
	else
	{
		FrameDataBuffer.SpriteProjectionMatrix = glm::ortho(0.0f, (float)FrameData.SpriteViewport->Width, (float)FrameData.SpriteViewport->Height, 0.0f);
	}

	for (auto Material : Materials)
	{
		UpdateFrameData(DeviceContext, Material->GetParameters(), FrameDataBuffer);
	}
}

void FMaterialManager::UpdateFrameData(ID3D11DeviceContext* DeviceContext, FMaterialParameters* Parameters, const FFrameDataBuffer& FrameDataBuffer)
{
	Parameters->UpdateUniform(DeviceContext, kFrameDataBufferName, (void*)&FrameDataBuffer, sizeof(FFrameDataBuffer));
}

void FMaterialManager::SetGlobalResourceView(const std::string& TextureName, ID3D11ShaderResourceView* ResourceView)
{
	for (auto Material : Materials)
	{
		Material->GetParameters()->SetResource(TextureName, ResourceView);
	}
}

void FMaterialManager::UploadGlobalUniformBuffer(const std::string& BufferName, void* BufferData, SizeType BufferSize)
{
	for (auto Material : Materials)
	{
		Material->GetParameters()->UpdateUniform(GraphicsContext->GetImmediateContext(), BufferName, BufferData, BufferSize);
	}
}

void FMaterialManager::RecompileMaterials(const DefinesVector& GlobalDefines /*= DefinesVector()*/)
{
	this->GlobalDefines = GlobalDefines;

	for (uint32_t MaterialIndex = 0; MaterialIndex < Materials.GetResourcesCount(); ++MaterialIndex)
	{
		CompileMaterial(Materials.GetResourceAtIndex(MaterialIndex), MaterialDescriptors[MaterialIndex]);
	}
}

FMaterial* FMaterialManager::GetMaterial(ConstantString Name) const
{
	return Materials.GetResource(Name);
}
