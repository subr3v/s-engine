#ifndef ForwardPlusRenderer_h__
#define ForwardPlusRenderer_h__

#include "Camera.h"
#include "MaterialManager.h"
#include "BaseScene.h"
#include <memory>
#include "ConfigVariable.h"
#include "SpriteBatch.h"

class FRenderTarget;
class FLightPass;
class FShadowMapping;
class FHighDynamicRange;
class FMaterial;
class FScreenSpaceAmbientOcclusion;

extern FConfigVariable ShadowMappingEnabled;
extern FConfigVariable SSAOEnabled;
extern FConfigVariable ShouldRecompileMaterials;
extern FConfigVariable ForceWireframe;

class FForwardPlusRenderer
{
public:
	FForwardPlusRenderer(class FGraphicsContext* GraphicsContext, class FMaterialManager* MaterialManager);
	~FForwardPlusRenderer();

	void Update(float DeltaTime);
	void DrawScene(const FCamera& Camera, class IScene* Scene);

private:
	void RecompileMaterials();
	void UpdateFrameData(const FCamera& Camera, class IScene* Scene);

	void DoScenePass(const FCamera& Camera, class IScene* Scene);
	void DoDepthNormalPrePass(const FCamera& Camera, class IScene* Scene);

	TUniquePtr<FLightPass> LightPass;
	TUniquePtr<FShadowMapping> ShadowMapping;
	TUniquePtr<FHighDynamicRange> HighDynamicRange;
	TUniquePtr<FScreenSpaceAmbientOcclusion> AmbientOcclusion;

	class FGraphicsContext* GraphicsContext;
	class FMaterialManager* MaterialManager;

	TUniquePtr<FRenderTarget> HighDynamicRangeMSAATarget;
	TUniquePtr<FRenderTarget> HighDynamicRangeTarget;
	TUniquePtr<FRenderTarget> NormalTarget;
	TUniquePtr<FRenderTarget> DepthTarget;
	TUniquePtr<FRenderTarget> OcclusionTarget;
	class FMaterial* DepthNormalMaterial;
	class FMaterial* DepthNormalBumpMaterial;

	TUniquePtr<FInstanceBuffer> InstanceBuffer;
	FViewport CurrentViewport;
	float TotalTime = 0.0f;
	float LastDeltaTime = 0.0f;

	static ConstantString kDepthNormalMaterial;
	static ConstantString kDepthNormalBumpMaterial;

	FMaterial* OverrideMaterialDepthNormalPass(FMaterial* CurrentMaterial);

	DefinesVector GenerateGlobalDefines();

#if DEBUG
	TUniquePtr<FSpriteBatch> SpriteBatch;
#endif
};

#endif // ForwardPlusRenderer_h__
