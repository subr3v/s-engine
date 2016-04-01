#ifndef ResourceManager_h__
#define ResourceManager_h__

#include <vector>
#include <string>
#include "ResourceContainer.h"

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "MaterialManager.h"

// Authoritative resource group, it allows you to specify a group of resources and define their scope globally so that you can
// dispose them all at the same time.
class FResourceGroup
{
public:
	FResourceGroup(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager);
	~FResourceGroup();

	FTexture* CreateTexture(const std::string& Name, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM, int bytesPerPixel = 4);
	FMesh* CreateMesh(const std::string& Name, const FMeshBuilder& MeshBuilder);
	FTexture* LoadTexture(const std::string& Filename, bool bIsGammaSpace = false);
	FModel* LoadModel(const std::string& Filename);
	void LoadPackage(const std::string& Filename);

	void UnloadResources();

	void SetBaseTexturePath(const std::string& BasePath) { BaseTexturePath = BasePath; }

	const TResourceContainer<FModel>& GetModels() const { return Models; }
	const TResourceContainer<FTexture>& GetTextures() const { return Textures; }
	const TResourceContainer<FMesh>& GetMeshes() const { return Meshes; }
private:
	FTexture* LoadTexture(const std::string& Name, IArchive* Archive, bool bIsGammaSpace = false);
	FModel* LoadModel(const std::string& Name, IArchive* Archive);

	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;

	TResourceContainer<FTexture> Textures;
	TResourceContainer<FModel> Models;
	TResourceContainer<FMesh> Meshes;

	std::string BaseTexturePath;
};


#endif // ResourceManager_h__
