#include "BinaryModelLoader.h"
#include <fstream>
#include "ModelData.h"
#include "Mesh.h"
#include "Texture.h"
#include "GraphicsContext.h"
#include "MaterialManager.h"
#include "ZipArchive.h"

FBinaryModelLoader::FBinaryModelLoader(FMaterialManager* MaterialManager, FGraphicsContext* GraphicsContext, FResourceGroup* ResourceGroup) : ResourceGroup(ResourceGroup), GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{

}

bool FBinaryModelLoader::LoadModel(IArchive* Archive, FModel& OutData)
{
	FModelData ModelData;
	ModelData.Serialize(FSerializer(Archive, true, false));

	FMeshBuilder MeshBuilder(sizeof(FModelVertex), ModelData.VertexCount, ModelData.Indices.Size());
	MeshBuilder.CopyIndices(ModelData.Indices);

	for (int32_t i = 0; i < ModelData.VertexCount; i++)
	{
		FModelVertex VertexData;

		VertexData.Position = ModelData.PositionChannel.Elements[i];
		VertexData.Normal = ModelData.NormalChannel.Elements[i];
		VertexData.UV = ModelData.DiffuseUVChannel.Elements[i];
		VertexData.Tangent = ModelData.TangentChannel.Elements[i];
		VertexData.BiTangent = ModelData.BiTangentChannel.Elements[i];

		MeshBuilder.CopyAddVertex(VertexData);
	}

	for (size_t i = 0; i < ModelData.Materials.Size(); i++)
	{
		const FMaterialData& MaterialData = ModelData.Materials[i];

		FMeshMaterialParameters Parameters;

		if (MaterialData.DiffuseTexturePath.size() > 0)
			Parameters.DiffuseTexture = ResourceGroup->LoadTexture(MaterialData.DiffuseTexturePath, true);

		if (MaterialData.NormalTexturePath.size() > 0)
			Parameters.NormalTexture = ResourceGroup->LoadTexture(MaterialData.NormalTexturePath);

		if (MaterialData.OpacityTexturePath.size() > 0)
			Parameters.OpacityTexture = ResourceGroup->LoadTexture(MaterialData.OpacityTexturePath);

		if (MaterialData.RoughnessTexturePath.size() > 0)
			Parameters.RoughnessTexture = ResourceGroup->LoadTexture(MaterialData.RoughnessTexturePath);

		if (MaterialData.RefractionTexturePath.size() > 0)
			Parameters.RefractionTexture = ResourceGroup->LoadTexture(MaterialData.RefractionTexturePath);

		Parameters.Albedo = MaterialData.DiffuseColour;
		Parameters.Roughness = MaterialData.Roughness;
		Parameters.RefractionIndex = MaterialData.Refraction;

		Parameters.Material = MaterialManager->GetMaterial(GetMaterialName(Parameters));

		MeshBuilder.AddMaterialParameters(Parameters);
	}

	for (auto MeshPart : ModelData.MeshData)
	{
		MeshBuilder.AddPart(FMeshPartData(MeshPart.IndexStart, MeshPart.IndexCount, MeshPart.MaterialIndex, MeshPart.BaseVertex));
	}

	OutData.Mesh = new FMesh(GraphicsContext, MeshBuilder);

	return true;
}

ConstantString FBinaryModelLoader::GetMaterialName(const FMeshMaterialParameters& Parameters)
{
	if (Parameters.DiffuseTexture != nullptr)
	{
		if (Parameters.NormalTexture != nullptr)
		{
			if (Parameters.RoughnessTexture != nullptr)
			{
				if (Parameters.RefractionTexture != nullptr)
				{
					return FCommonMaterials::kDiffuseNormalMappedRoughnessRefraction;
				}
				else
				{
					return FCommonMaterials::kDiffuseNormalMappedRoughness;
				}
			}
			else
			{
				return FCommonMaterials::kDiffuseNormalMapped;
			}
		}
		else
		{
			if (Parameters.RoughnessTexture != nullptr)
			{
				if (Parameters.RefractionTexture != nullptr)
				{
					return FCommonMaterials::kDiffuseRoughnessRefraction;
				}
				else
				{
					return FCommonMaterials::kDiffuseRoughness;
				}
			}
			else
			{
				return FCommonMaterials::kDiffuse;
			}
		}
	}
	else
	{
		return FCommonMaterials::kFlatColour;
	}
}
