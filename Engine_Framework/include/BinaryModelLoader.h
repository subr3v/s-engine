#ifndef BinaryModelLoader_h__
#define BinaryModelLoader_h__

#include "Model.h"
#include "MaterialManager.h"
#include "ResourceManager.h"

class FBinaryModelLoader
{
public:
	FBinaryModelLoader(FMaterialManager* MaterialManager, FGraphicsContext* GraphicsContext, FResourceGroup* ResourceGroup);
	bool LoadModel(IArchive* Archive, FModel& OutData);

private:
	FResourceGroup* ResourceGroup;
	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;

	ConstantString GetMaterialName(const FMeshMaterialParameters& Parameters);
};

#endif // BinaryModelLoader_h__
