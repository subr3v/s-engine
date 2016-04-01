#ifndef MaterialManager_h__
#define MaterialManager_h__

#include <glm\glm.hpp>
#include "Vector.h"
#include <string>
#include <d3d11.h>
#include "Material.h"
#include "MaterialsData.h"
#include "ResourceContainer.h"
#include "CommonMaterials.h"
#include "MaterialsConfiguration.h"

class FMaterial;

class FMaterialManager
{
public:
	FMaterialManager(class FGraphicsContext* GraphicsContext);
	~FMaterialManager();

	void LoadMaterials(const DefinesVector& GlobalDefines = DefinesVector());
	void RecompileMaterials(const DefinesVector& GlobalDefines = DefinesVector());

	void UploadGlobalUniformBuffer(const std::string& BufferName, void* BufferData, SizeType BufferSize);
	void UpdateMaterialBuffers(const FRenderFrameData& FrameData, class FGraphicsContext* GraphicsContext);
	void SetGlobalResourceView(const std::string& TextureName, ID3D11ShaderResourceView* ResourceView);

	FMaterial* GetMaterial(ConstantString Name) const;

	// Helper functions: might want to remove them later on, too implementation specific..
	FMaterial* GetSpriteMaterial() const { return GetMaterial(FCommonMaterials::kSprite); }
	FMaterial* GetPointSpriteMaterial() const { return GetMaterial(FCommonMaterials::kPointSprite); }
	FMaterial* GetDiffuseMaterial() const { return GetMaterial(FCommonMaterials::kDiffuse); }
	FMaterial* GetNormalDiffuseMaterial() const { return GetMaterial(FCommonMaterials::kDiffuseNormalMapped); }
	FMaterial* GetFlatColourMaterial() const { return GetMaterial(FCommonMaterials::kFlatColour); }
	FMaterial* GetDepthMaterial() const { return GetMaterial(FCommonMaterials::kDepthOnly); }
	FMaterial* GetFullscreenColour() const { return GetMaterial(FCommonMaterials::kFullscreenColour); }
	FMaterial* GetLightPassMaterial() const { return GetMaterial(FCommonMaterials::kDeferredRenderingLightPass); }
	FMaterial* GetCombinePassMaterial() const { return GetMaterial(FCommonMaterials::kDeferredRenderingCombine); }

	static ConstantString kFrameDataBufferName;
private:
	FMaterial* CreateMaterial(class FGraphicsContext* GraphicsContext, const FMaterialDescriptor& MaterialDescriptor);
	void CompileMaterial(FMaterial* Material, const FMaterialDescriptor& Descriptor);

	void UpdateFrameData(ID3D11DeviceContext* DeviceContext, FMaterialParameters* Parameters, const struct FFrameDataBuffer& FrameDataBuffer);

	class FGraphicsContext* GraphicsContext;

	TResourceContainer<FMaterial> Materials;
	TVector<FMaterialDescriptor> MaterialDescriptors;

	DefinesVector GlobalDefines;
};


#endif // MaterialManager_h__
