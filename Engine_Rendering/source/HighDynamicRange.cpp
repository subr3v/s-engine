#include "HighDynamicRange.h"
#include "GraphicsContext.h"
#include "RenderTarget.h"
#include "MaterialManager.h"
#include "MemoryUtils.h"
#include "ForwardPlusRenderer.h"
#include "PostProcessing.h"
#include "SpriteBatch.h"
#include "ConstantBufferTypes.h"

static ConstantString kDownsampleMaterial = "Downsample";
static ConstantString kOneAxisBlurMaterial = "OneAxisBlur";
static ConstantString kThresholdMaterial = "ThresholdLuminance";
static ConstantString kToneMapMaterial = "ToneMap";
static ConstantString kLuminanceAdaptionMaterial = "LuminanceAdaption";
static ConstantString kBloomTexture = "BloomTexture";
static ConstantString kOriginalTextureName = "OriginalTexture";
static ConstantString kBlurBufferName = "BlurBuffer";
static ConstantString kLuminanceMaterial = "Luminance";
static ConstantString kLuminanceTextureName = "LuminanceTexture";
static ConstantString kCurrentLuminanceTextureName = "CurrentLuminanceTexture";
static ConstantString kPreviousLuminanceTextureName = "PreviousLuminanceTexture";
static ConstantString kSettingsBufferName = "HdrSettings";
static ConstantString kBloomSettingsBufferName = "BloomSettings";

static FConfigVariable BloomThreshold("Forward+", "Bloom Threshold", 0.4f);
static FConfigVariable BloomStrength("Forward+", "Bloom Strength", 1.0f);

FHighDynamicRange::FHighDynamicRange(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{
	BlurBuffer = Make_Unique<FBlurBuffer>();
	SettingsBuffer = Make_Unique<FHdrSettings>();
	bCurrentLuminanceTarget = false;
}

FHighDynamicRange::~FHighDynamicRange()
{
	ReleaseResources();
}

void FHighDynamicRange::Initialise()
{
	ReleaseResources();

	DXGI_FORMAT TextureFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT LuminanceFormat = DXGI_FORMAT_R32_FLOAT;

	int BloomWidth = GraphicsContext->GetWidth() / 2;
	int BloomHeight = GraphicsContext->GetHeight() / 2;

	int Width = GraphicsContext->GetWidth();
	int Height = GraphicsContext->GetHeight();
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	LuminanceTexture = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, 1024, 1024, LuminanceFormat, LuminanceFormat, LuminanceFormat, true);
	for (int i = 0; i < 2; i++)
	{
		AdaptedLuminanceTarget[i] = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, 1, 1, LuminanceFormat, LuminanceFormat);
	}
	DownSampleTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, BloomWidth, BloomHeight, TextureFormat, TextureFormat);
	HorizontalBlurTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, BloomWidth, BloomHeight, TextureFormat, TextureFormat);
	VerticalBlurTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, BloomWidth, BloomHeight, TextureFormat, TextureFormat);

	DownSampleTarget->Clear(DeviceContext);
	HorizontalBlurTarget->Clear(DeviceContext);
	VerticalBlurTarget->Clear(DeviceContext);

	DownSampleMaterial = MaterialManager->GetMaterial(kDownsampleMaterial);
	BlurMaterial = MaterialManager->GetMaterial(kOneAxisBlurMaterial);
	TonemapMaterial = MaterialManager->GetMaterial(kToneMapMaterial);
	LuminanceMaterial = MaterialManager->GetMaterial(kLuminanceMaterial);
	LuminanceAdaptionMaterial = MaterialManager->GetMaterial(kLuminanceAdaptionMaterial);
	ThresholdMaterial = MaterialManager->GetMaterial(kThresholdMaterial);

	UpdateSettings();
}

void FHighDynamicRange::UpdateSettings()
{
	SettingsBuffer->BloomStrength = BloomStrength.AsFloat();
	TonemapMaterial->GetParameters()->UpdateUniform(GraphicsContext->GetImmediateContext(), kSettingsBufferName, SettingsBuffer.Get(), sizeof(FHdrSettings));
	float BloomThresholdValue = BloomThreshold.AsFloat();
	ThresholdMaterial->GetParameters()->UpdateUniform(GraphicsContext->GetImmediateContext(), kBloomSettingsBufferName, &BloomThresholdValue, sizeof(float));
}

void FHighDynamicRange::ReleaseResources()
{
}

void FHighDynamicRange::Apply(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	LuminanceAdaptation(CurrentViewport, HdrTarget, FinalTarget);
	Bloom(CurrentViewport, HdrTarget, FinalTarget);

	// Final Tone mapping
	TonemapMaterial->GetParameters()->SetResource(kOriginalTextureName, HdrTarget->GetRenderResource());
	TonemapMaterial->GetParameters()->SetResource(kBloomTexture, VerticalBlurTarget->GetRenderResource());
	TonemapMaterial->GetParameters()->SetResource(kLuminanceTextureName, AdaptedLuminanceTarget[bCurrentLuminanceTarget]->GetRenderResource());
	FPostProcessing::Apply(GraphicsContext, CurrentViewport, nullptr, FinalTarget, TonemapMaterial);

	bCurrentLuminanceTarget = !bCurrentLuminanceTarget;
}

void FHighDynamicRange::DebugHdr(FSpriteBatch* SpriteBatch)
{
	const float kVerticalSpacing = 64 + 4;
	SpriteBatch->DrawSprite(FSprite(LuminanceTexture->GetRenderResource(), 64, 64, 64, 64));
	SpriteBatch->DrawSprite(FSprite(AdaptedLuminanceTarget[0]->GetRenderResource(), 64, 64 + kVerticalSpacing, 64, 64));
	SpriteBatch->DrawSprite(FSprite(AdaptedLuminanceTarget[1]->GetRenderResource(), 64, 64 + kVerticalSpacing * 2, 64, 64));

	const float kHorizontalSpacing = 128 + 8;
	SpriteBatch->DrawSprite(FSprite(DownSampleTarget->GetRenderResource(), 132, 64, 128, 128));
	SpriteBatch->DrawSprite(FSprite(HorizontalBlurTarget->GetRenderResource(), 132 + kHorizontalSpacing, 64, 128, 128));
	SpriteBatch->DrawSprite(FSprite(VerticalBlurTarget->GetRenderResource(), 132 + kHorizontalSpacing * 2, 64, 128, 128));
}

void FHighDynamicRange::Bloom(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget)
{
	// Downsample and suppress ldr info
	ThresholdMaterial->GetParameters()->SetResource(kLuminanceTextureName, AdaptedLuminanceTarget[bCurrentLuminanceTarget]->GetRenderResource());
	FPostProcessing::Apply(GraphicsContext, CurrentViewport, HdrTarget->GetRenderResource(), DownSampleTarget->GetRenderTarget(), ThresholdMaterial, (float)DownSampleTarget->GetWidth(), (float)DownSampleTarget->GetHeight());

	Blur(CurrentViewport, DownSampleTarget.Get(), HorizontalBlurTarget.Get(), VerticalBlurTarget.Get());
	Blur(CurrentViewport, VerticalBlurTarget.Get(), DownSampleTarget.Get(), HorizontalBlurTarget.Get());
	Blur(CurrentViewport, HorizontalBlurTarget.Get(), VerticalBlurTarget.Get(), DownSampleTarget.Get());
	Blur(CurrentViewport, DownSampleTarget.Get(), HorizontalBlurTarget.Get(), VerticalBlurTarget.Get());
}

void FHighDynamicRange::LuminanceAdaptation(FViewport CurrentViewport, FRenderTarget* HdrTarget, ID3D11RenderTargetView* FinalTarget)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	// Calculate average luminance in the scene through mip map generation
	FPostProcessing::Apply(GraphicsContext, CurrentViewport, HdrTarget->GetRenderResource(), LuminanceTexture->GetRenderTarget(), LuminanceMaterial, 1024, 1024);
	LuminanceTexture->GenerateMips(DeviceContext);
	LuminanceAdaptionMaterial->GetParameters()->SetResource(kPreviousLuminanceTextureName, AdaptedLuminanceTarget[!bCurrentLuminanceTarget]->GetRenderResource());
	LuminanceAdaptionMaterial->GetParameters()->SetResource(kCurrentLuminanceTextureName, LuminanceTexture->GetRenderResource());

	FViewport Viewport(0, 0, 1, 1);
	FPostProcessing::Apply(GraphicsContext, Viewport, nullptr, AdaptedLuminanceTarget[bCurrentLuminanceTarget]->GetRenderTarget(), LuminanceAdaptionMaterial, 1, 1);
}

void FHighDynamicRange::Blur(FViewport CurrentViewport, FRenderTarget* SourceTarget, FRenderTarget* IntermediateTarget, FRenderTarget* DestinationTarget)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	float BloomWidth = (float)DestinationTarget->GetWidth();
	float BloomHeight = (float)DestinationTarget->GetHeight();

	// Horizontal Blur
	BlurBuffer->TextureOffsetU = 1.0f / (BloomWidth);
	BlurBuffer->TextureOffsetV = 0.0f;
	BlurMaterial->GetParameters()->UpdateUniform(DeviceContext, kBlurBufferName, BlurBuffer.Get(), sizeof(FBlurBuffer));
	FPostProcessing::Apply(GraphicsContext, CurrentViewport, SourceTarget->GetRenderResource(), IntermediateTarget->GetRenderTarget(), BlurMaterial, BloomWidth, BloomHeight);

	// Vertical Blur
	BlurBuffer->TextureOffsetU = 0.0f;
	BlurBuffer->TextureOffsetV = 1.0f / (BloomHeight);
	BlurMaterial->GetParameters()->UpdateUniform(DeviceContext, kBlurBufferName, BlurBuffer.Get(), sizeof(FBlurBuffer));
	FPostProcessing::Apply(GraphicsContext, CurrentViewport, IntermediateTarget->GetRenderResource(), DestinationTarget->GetRenderTarget(), BlurMaterial, BloomWidth, BloomHeight);
}
