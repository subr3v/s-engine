#ifndef ScreenSpaceAmbientOcclusion_h__
#define ScreenSpaceAmbientOcclusion_h__

#include <string>
#include "Camera.h"
#include "UniquePtr.h"
#include "MemoryUtils.h"

class FMaterial;
class FMaterialManager;
class FGraphicsContext;
class FTexture;
class FRenderTarget;
class FDeferredRenderer;


#define alignas(size) __declspec( align(size) )

#pragma pack(push,4)

struct alignas(16) FAmbientOcclusionBuffer : public FGraphicsAlignedBuffer
{
	static const int kKernelSize = 16;

	glm::vec4 Kernel[kKernelSize];
	float KernelSize; float Radius; float DistanceThreshold; float Padding;

	static ConstantString kBufferName;
};

#pragma pack(pop) 


class FScreenSpaceAmbientOcclusion
{
public:
	FScreenSpaceAmbientOcclusion(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager);
	~FScreenSpaceAmbientOcclusion();

	void Initialise();
	void Apply(FViewport CurrentViewport, FRenderTarget* DepthBuffer, FRenderTarget* NormalBuffer, FRenderTarget* OcclusionBuffer);

	static ConstantString kAmbientOcclusionMap;
	static ConstantString kNormalBufferMap;
private:

	static FTexture* GenerateNoiseTexture(FGraphicsContext* GraphicsContext, int Width, int Height);

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;
	FMaterial* Material;

	TUniquePtr<FAmbientOcclusionBuffer> AmbientOcclusionBuffer;

	static ConstantString kAmbientOcclusionBufferName;
	static ConstantString kAmbientOcclusionMaterial;

	static const float kAmbientOcclusionRadius;
	static const float kAmbientOcclusionDistanceThreshold;
};


#endif // ScreenSpaceAmbientOcclusion_h__
