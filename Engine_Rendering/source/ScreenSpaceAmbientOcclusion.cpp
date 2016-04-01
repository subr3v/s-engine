#include "ScreenSpaceAmbientOcclusion.h"
#include "ConstantBufferTypes.h"
#include <glm/glm.hpp>
#include <d3d11.h>
#include "Vector.h"
#include "glm/gtx/compatibility.hpp"
#include "Texture.h"
#include "MaterialManager.h"
#include "GraphicsContext.h"
#include "RenderTarget.h"
#include "PostProcessing.h"
#include "ConfigVariable.h"

ConstantString FScreenSpaceAmbientOcclusion::kAmbientOcclusionBufferName = "AmbientOcclusionBuffer";
ConstantString FScreenSpaceAmbientOcclusion::kAmbientOcclusionMaterial = "AmbientOcclusion";
ConstantString FScreenSpaceAmbientOcclusion::kAmbientOcclusionMap = "OcclusionTexture";
ConstantString FScreenSpaceAmbientOcclusion::kNormalBufferMap = "NormalBuffer";

static FConfigVariable ScreenSpaceAmbientOcclusionRadius("Forward+", "SSAO Radius", 0.015f);
static FConfigVariable ScreenSpaceAmbientOcclusionDistanceThreshold("Forward+", "SSAO Distance Threshold", 1500.0f);

static void GenerateAmbientOcclusionBuffer(FAmbientOcclusionBuffer* Buffer)
{
	Buffer->KernelSize = FAmbientOcclusionBuffer::kKernelSize;
	
	// Poisson disk samples
	Buffer->Kernel[0] = glm::vec4(-0.94201624, -0.39906216, 0.0, 0.0);
	Buffer->Kernel[1] = glm::vec4(0.94558609, -0.76890725, 0.0, 0.0);
	Buffer->Kernel[2] = glm::vec4(-0.094184101, -0.92938870, 0.0, 0.0);
	Buffer->Kernel[3] = glm::vec4(0.34495938, 0.29387760, 0.0, 0.0);
	Buffer->Kernel[4] = glm::vec4(-0.91588581, 0.45771432, 0.0, 0.0);
	Buffer->Kernel[5] = glm::vec4(-0.81544232, -0.87912464, 0.0, 0.0);
	Buffer->Kernel[6] = glm::vec4(-0.38277543, 0.27676845, 0.0, 0.0);
	Buffer->Kernel[7] = glm::vec4(0.97484398, 0.75648379, 0.0, 0.0);
	Buffer->Kernel[8] = glm::vec4(0.44323325, -0.97511554, 0.0, 0.0);
	Buffer->Kernel[9] = glm::vec4(0.53742981, -0.47373420, 0.0, 0.0);
	Buffer->Kernel[10] = glm::vec4(-0.26496911, -0.41893023, 0.0, 0.0);
	Buffer->Kernel[11] = glm::vec4(0.79197514, 0.19090188, 0.0, 0.0);
	Buffer->Kernel[12] = glm::vec4(-0.24188840, 0.99706507, 0.0, 0.0);
	Buffer->Kernel[13] = glm::vec4(-0.81409955, 0.91437590, 0.0, 0.0);
	Buffer->Kernel[14] = glm::vec4(0.19984126, 0.78641367, 0.0, 0.0);
	Buffer->Kernel[15] = glm::vec4(0.14383161, -0.14100790, 0.0, 0.0);
}

FScreenSpaceAmbientOcclusion::FScreenSpaceAmbientOcclusion(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{
	AmbientOcclusionBuffer = Make_Unique<FAmbientOcclusionBuffer>();
}

FScreenSpaceAmbientOcclusion::~FScreenSpaceAmbientOcclusion()
{

}

void FScreenSpaceAmbientOcclusion::Initialise()
{
	Material = MaterialManager->GetMaterial(kAmbientOcclusionMaterial);

	GenerateAmbientOcclusionBuffer(AmbientOcclusionBuffer.Get());
	AmbientOcclusionBuffer->Radius = ScreenSpaceAmbientOcclusionRadius.AsFloat();
	AmbientOcclusionBuffer->DistanceThreshold = ScreenSpaceAmbientOcclusionDistanceThreshold.AsFloat();

	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	Material->GetParameters()->UpdateUniform(DeviceContext, kAmbientOcclusionBufferName, AmbientOcclusionBuffer.Get(), sizeof(FAmbientOcclusionBuffer));
}

void FScreenSpaceAmbientOcclusion::Apply(FViewport CurrentViewport, FRenderTarget* DepthBuffer, FRenderTarget* NormalBuffer, FRenderTarget* OcclusionBuffer)
{
	ID3D11RenderTargetView* RenderTarget = OcclusionBuffer->GetRenderTarget();
	Material->GetParameters()->SetResource(kNormalBufferMap, NormalBuffer->GetRenderResource());

	FPostProcessing::Apply(GraphicsContext, CurrentViewport, DepthBuffer->GetRenderResource(), RenderTarget, Material);
}
