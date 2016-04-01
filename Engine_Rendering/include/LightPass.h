#ifndef LightPass_h__
#define LightPass_h__

#include "Camera.h"
#include <string>
#include "BaseScene.h"
#include <glm/glm.hpp>
#include "UniquePtr.h"
#include "MemoryUtils.h"
#include "ComputeShader.h"

class FRenderTarget;
class FForwardPlusRenderer;
class FGraphicsContext;
class FMaterialManager;
class FMaterial;
class IScene;
class FShadowMapping;

class FLightPass
{
public:
	FLightPass(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager);
	~FLightPass();

	void Initialise();
	void Apply(FForwardPlusRenderer* Renderer, FShadowMapping* ShadowMap, FRenderTarget* DepthTarget, IScene* Scene, const FCamera& Camera);

private:
	void UploadBuffers(IScene* Scene, const FCamera& Camera, FShadowMapping* ShadowMap, FRenderTarget* DepthTarget);

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;
	
	TUniquePtr<FComputeShader> LightCullingShader;

	// Input buffers for the compute shader, they are also used by the BRDF shader to evaluate light information
	TUniquePtr<FGraphicsBuffer> PointLightGeometryBuffer;
	TUniquePtr<FGraphicsBuffer> SpotLightGeometryBuffer;

	// Buffer that holds the output information from the compute shader
	TUniquePtr<FGraphicsBuffer> TileToLightIndexMap;

	TUniquePtr<FGraphicsBuffer> PointLightParameterBuffer; // Those are only needed by the BRDF shader
	TUniquePtr<FGraphicsBuffer> SpotLightParameterBuffer; // Those are only needed by the BRDF shader

	struct FLightBuffer : FGraphicsAlignedBuffer
	{
		glm::vec4 AmbientLight;
	};

	TUniquePtr<FLightBuffer> LightBuffer;
};

#endif // LightPass_h__
