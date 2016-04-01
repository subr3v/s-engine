#include "ResourceManager.h"
#include "BinaryModelLoader.h"
#include "GraphicsContext.h"
#include "ZipArchive.h"
#include "ResourcePackage.h"


FResourceGroup::FResourceGroup(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{

}

FResourceGroup::~FResourceGroup()
{

}

FTexture* FResourceGroup::CreateTexture(const std::string& Name, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/, int bytesPerPixel /*= 4*/)
{
	FTexture* Texture = new FTexture(GraphicsContext, Width, Height, Data, TextureFormat, bytesPerPixel);
	Textures.AddResource(Name, Texture);
	Texture->SetName(Name);
	return Texture;
}

FTexture* FResourceGroup::LoadTexture(const std::string& Filename, bool bIsGammaSpace)
{
	FTexture* Texture = Textures.GetResource(Filename);
	
	if (Texture == nullptr)
	{
		TUniquePtr<FZipArchive> Archive = Make_Unique<FZipArchive>(EZipArchiveMode::Read);
		if (Archive->OpenFile(BaseTexturePath + Filename))
		{
			Archive->OpenFirstSubFile();
			Texture = LoadTexture(Filename, Archive.Get(), bIsGammaSpace);
		}
		else
		{
			return nullptr;
		}
	}

	return Texture;
}

FTexture* FResourceGroup::LoadTexture(const std::string& Name, IArchive* Archive, bool bIsGammaSpace /*= false*/)
{
	FTexture* Texture = new FTexture(GraphicsContext, Archive, bIsGammaSpace);
	Textures.AddResource(Name, Texture);
	return Texture;
}

FModel* FResourceGroup::LoadModel(const std::string& Filename)
{
	FModel* Model = Models.GetResource(Filename);
	if (Model == nullptr)
	{
		TUniquePtr<FZipArchive> Archive = Make_Unique<FZipArchive>(EZipArchiveMode::Read);
		if (Archive->OpenFile(Filename))
		{
			Archive->OpenFirstSubFile();
			Model = LoadModel(Filename, Archive.Get());
		}
	}

	return Model;
}

FModel* FResourceGroup::LoadModel(const std::string& Name, IArchive* Archive)
{
	FModel* Model = new FModel();
	FBinaryModelLoader Loader(MaterialManager, GraphicsContext, this);
	Loader.LoadModel(Archive, *Model);
	Models.AddResource(Name, Model);
	return Model;
}

FMesh* FResourceGroup::CreateMesh(const std::string& Name, const FMeshBuilder& MeshBuilder)
{
	FMesh* Mesh = new FMesh(GraphicsContext, MeshBuilder);
	Meshes.AddResource(Name, Mesh);
	return Mesh;
}

void FResourceGroup::UnloadResources()
{
	Models.UnloadResources();
	Meshes.UnloadResources();
	Textures.UnloadResources();
}

void FResourceGroup::LoadPackage(const std::string& Filename)
{
	FResourcePackage Package(Filename);
	const TVector<FResource>& Resources = Package.GetResources();

	for (auto Resource : Resources)
	{
		IArchive* Archive = Resource.GetResourceArchive(&Package);
		if (Resource.GetType() == "Texture")
		{
			LoadTexture(Resource.GetPath(), Archive);
		}
		else if (Resource.GetType() == "3D Model")
		{
			LoadModel(Resource.GetPath(), Archive);
		}
		delete Archive;
	}
}
