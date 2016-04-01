#ifndef ShadowMapping_h__
#define ShadowMapping_h__

#include "RenderTarget.h"
#include "ResourceContainer.h"
#include <glm/glm.hpp>
#include "BaseScene.h"
#include "Camera.h"
#include "MemoryUtils.h"
#include "UniquePtr.h"

#define alignas(size) __declspec( align(size) )

#pragma pack(push,4)

struct alignas(16) FShadowBuffer : public FGraphicsAlignedBuffer
{
	static const int kMaxShadowMapNum = 4;
	glm::mat4 LightViewProjectionMatrix[kMaxShadowMapNum];
	glm::mat4 LightViewMatrix[kMaxShadowMapNum];
	glm::vec4 LightTextureOffsets[kMaxShadowMapNum];

	static ConstantString kBufferName;
};

struct alignas(16) FShadowInstanceBuffer
{
	int CurrentShadowMap;

	static ConstantString kBufferName;
};

#pragma pack(pop) 

class FShadowMapping
{
public:
	FShadowMapping(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager);
	~FShadowMapping();

	void Initialise();
	void GatherShadowMaps(class IScene* Scene);

	glm::ivec2 GetShadowMapResolution() const { return ShadowMapResolution; }
	void SetShadowMapResolution(glm::ivec2 Resolution) { ShadowMapResolution = Resolution; }

	void UploadShadowBuffer(ID3D11DeviceContext* DeviceContext);
	ID3D11ShaderResourceView* GetShadowMap();

	static ConstantString kShadowMap;
private:
	void RenderShadowMap(class IScene* Scene, FRenderTarget* Target, int ShadowIndex);
	void UpdateShadowBuffer(class IScene* Scene);

	void GenerateSpotLightMatrices(const FSpotlight& SpotLight, glm::mat4& OutViewMatrix, glm::mat4& OutViewProjectionMatrix);
	FViewport GenerateViewportFromIndex(int Index);

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;
	class FMaterial* DepthOnlyMaterial;
	TUniquePtr<FRenderTarget> ShadowMap;
	TUniquePtr<FShadowBuffer> ShadowBuffer;

	glm::ivec2 ShadowMapResolution = glm::ivec2(4096, 4096);
};

#endif // ShadowMapping_h__
