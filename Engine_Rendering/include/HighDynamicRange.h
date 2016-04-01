#ifndef HighDynamicRange_h__
#define HighDynamicRange_h__

#include "Vector.h"
#include <string>
#include <d3d11.h>
#include "Camera.h"
#include "UniquePtr.h"
#include "MemoryUtils.h"

class FGraphicsContext;
class FMaterialManager;
class FMaterial;
class FRenderTarget;
class FForwardPlusRenderer;

struct FBlurBuffer : public FGraphicsAlignedBuffer
{
	float TextureOffsetU;
	float TextureOffsetV;
	glm::vec2 Padding;
};

struct FHdrSettings : public FGraphicsAlignedBuffer
{
	float BloomStrength;
	glm::vec3 Padding;
};

class FHighDynamicRange
{
public:
	FHighDynamicRange(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager);
	~FHighDynamicRange();

	void Initialise();
	void Apply(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget);

	void UpdateSettings();

	void DebugHdr(class FSpriteBatch* SpriteBatch);
private:
	void ReleaseResources();

	void LuminanceAdaptation(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget);
	void Bloom(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget);

	void Blur(FViewport CurrentViewport, FRenderTarget* SourceTarget, FRenderTarget* IntermediateTarget, FRenderTarget* DestinationTarget);

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;

	TUniquePtr<FBlurBuffer> BlurBuffer;
	TUniquePtr<FHdrSettings> SettingsBuffer;

	TUniquePtr<FRenderTarget> LuminanceTexture;
	TUniquePtr<FRenderTarget> AdaptedLuminanceTarget[2];
	TUniquePtr<FRenderTarget> DownSampleTarget;
	TUniquePtr<FRenderTarget> HorizontalBlurTarget;
	TUniquePtr<FRenderTarget> IntermediateBlurTarget;
	TUniquePtr<FRenderTarget> VerticalBlurTarget;

	bool bCurrentLuminanceTarget;

	FMaterial* ThresholdMaterial = nullptr;
	FMaterial* DownSampleMaterial = nullptr;
	FMaterial* BlurMaterial = nullptr;
	FMaterial* TonemapMaterial = nullptr;
	FMaterial* LuminanceMaterial = nullptr;
	FMaterial* LuminanceAdaptionMaterial = nullptr;
};

#endif // HighDynamicRange_h__
