#include "ModelConverter.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "ModelData.h"
#include <iostream>
#include "Utils.h"

bool IsExtensionSupported(const std::string& Extension)
{
	return Extension == ".fbx" || Extension == ".obj" || Extension == ".dae";
}

const char* GetTypeName()
{
	return "3D Model";
}

static glm::vec3 ToGlmVector(aiVector3D Vector) { return glm::vec3(Vector.x, Vector.y, Vector.z); }
static glm::vec2 ToGlmVector2(aiVector3D Vector) { return glm::vec2(Vector.x, Vector.y); }
static glm::vec2 ToGlmVector(aiVector2D Vector) { return glm::vec2(Vector.x, Vector.y); }
static glm::vec3 ToGlmVector(aiColor3D Colour) { return glm::vec3(Colour.r, Colour.g, Colour.b); }

static aiMatrix4x4 GetGlobalTransform(aiNode* Node)
{
	aiMatrix4x4 Transform = Node->mTransformation;
	
	aiNode* CurrentNode = Node->mParent;
	while (CurrentNode != nullptr)
	{
		Transform = CurrentNode->mTransformation * Transform;
		CurrentNode = CurrentNode->mParent;
	}

	return Transform;
}

struct FAssimpNode
{
	aiNode* Node;
	// Also other stuff here

	FAssimpNode(aiNode* Node)
	{
		this->Node = Node;
	}
};

struct FImportedModelVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
	glm::vec2 UV;
	// Add more stuff
};

struct FImportedMesh
{
	s32 IndexStart = 0;
	s32 IndexCount = 0;
	s16 MaterialIndex = 0;
	s32 BaseVertex = 0;
};

struct FImportedModel
{
	void BeginSubMesh(int MaterialIndex)
	{
		if (CurrentMesh != nullptr)
		{
			IndexOffset += CurrentMesh->IndexCount;
		}

		Meshes.Add(FImportedMesh());
		CurrentMesh = &Meshes[Meshes.Size() - 1];

		CurrentMesh->BaseVertex = BaseVertex;
		CurrentMesh->IndexCount = 0;
		CurrentMesh->IndexStart = IndexOffset;
		CurrentMesh->MaterialIndex = MaterialIndex;
	}

	FImportedModelVertex& AddVertex()
	{
		++BaseVertex;
		Vertices.Add(FImportedModelVertex());
		return Vertices[Vertices.Size() - 1];
	}

	FImportedModelVertex& GetVertex(u32 Index)
	{
		return Vertices[CurrentMesh->BaseVertex + Index];
	}

	void AddIndex(u32 Index)
	{
		Indices.Add(Index);
		++CurrentMesh->IndexCount;
	}

	TVector<u32> Indices;
	TVector<FImportedModelVertex> Vertices;
	TVector<FImportedMesh> Meshes;

private:
	FImportedMesh* CurrentMesh = nullptr;
	int32_t IndexOffset = 0;
	int32_t BaseVertex = 0;
};

static void LineariseScene(const aiScene* Scene, aiNode* Node, TVector<FAssimpNode>& LinearArray)
{
	LinearArray.Add(FAssimpNode(Node));
	for (size_t i = 0; i < Node->mNumChildren; ++i)
	{
		LineariseScene(Scene, Node->mChildren[i], LinearArray);
	}
}

static void CreateMeshData(aiNode* Node, aiMesh* Mesh, FImportedModel& ModelData)
{
	if (Mesh->mNumFaces <= 0)
		return;

	if (Mesh->mFaces[0].mNumIndices != 3)
		return;

	ModelData.BeginSubMesh(Mesh->mMaterialIndex);

	for (size_t Index = 0; Index < Mesh->mNumFaces; ++Index)
	{
		const aiFace& Face = Mesh->mFaces[Index];
		if (Face.mNumIndices != 3)
			continue;

		ModelData.AddIndex(Face.mIndices[0]);
		ModelData.AddIndex(Face.mIndices[1]);
		ModelData.AddIndex(Face.mIndices[2]);
	}

	aiMatrix4x4 PositionTransform = GetGlobalTransform(Node);
	aiMatrix4x4 WorldTransform = PositionTransform;
	WorldTransform.Inverse().Transpose();

	aiMatrix3x3 NormalTransform = aiMatrix3x3(WorldTransform);

	// Cache friendly loops
	for (size_t Vertex = 0; Vertex < Mesh->mNumVertices; ++Vertex)
	{
		FImportedModelVertex& VertexData = ModelData.AddVertex();
		VertexData.Position = ToGlmVector(PositionTransform * Mesh->mVertices[Vertex]);
	}

	if (Mesh->mNormals != nullptr)
	{
		for (size_t Vertex = 0; Vertex < Mesh->mNumVertices; ++Vertex)
		{
			FImportedModelVertex& VertexData = ModelData.GetVertex(Vertex);
			VertexData.Normal = ToGlmVector(NormalTransform * Mesh->mNormals[Vertex]);
		}
	}
	
	if (Mesh->mTangents != nullptr)
	{
		for (size_t Vertex = 0; Vertex < Mesh->mNumVertices; ++Vertex)
		{
			FImportedModelVertex& VertexData = ModelData.GetVertex(Vertex);
			VertexData.Tangent = ToGlmVector(Mesh->mTangents[Vertex]);
		}
	}

	if (Mesh->mBitangents != nullptr)
	{
		for (size_t Vertex = 0; Vertex < Mesh->mNumVertices; ++Vertex)
		{
			FImportedModelVertex& VertexData = ModelData.GetVertex(Vertex);
			VertexData.BiTangent = ToGlmVector(Mesh->mBitangents[Vertex]);
		}
	}

	if (Mesh->mTextureCoords != nullptr && Mesh->mTextureCoords[0] != nullptr)
	{
		for (size_t Vertex = 0; Vertex < Mesh->mNumVertices; ++Vertex)
		{
			FImportedModelVertex& VertexData = ModelData.GetVertex(Vertex);
			VertexData.UV = ToGlmVector2(Mesh->mTextureCoords[0][Vertex]);
		}
	}
}

bool ImportTexture(aiMaterial* Material, aiTextureType TextureType, const std::string& TextureTypeName, std::string& TargetPath)
{
	aiString Path;
	aiTextureMapMode TextureMapMode;
	unsigned int UvIndex;
	float Blend;
	aiTextureOp TextureOp;
	aiTextureMapping TextureMapping;

	aiReturn Result = Material->GetTexture(TextureType, 0, &Path, &TextureMapping, &UvIndex, &Blend, &TextureOp, &TextureMapMode);

	if (Result == aiReturn_SUCCESS)
	{
		TargetPath = GetNameFromPath(Path.C_Str());
		std::cout << "Found " << TextureTypeName << " texture, path: " << TargetPath << std::endl;
		return true;
	}

	return false;
}

bool ConvertAsset(const FAsset& Asset, FSerializer& Serializer)
{
	Assimp::Importer Importer;

	const aiScene* Scene = Importer.ReadFile(Asset.GetFilename(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_JoinIdenticalVertices  | aiProcess_ConvertToLeftHanded);
	
	if (Scene == nullptr)
		return false;

	TVector<FAssimpNode> Nodes;
	LineariseScene(Scene, Scene->mRootNode, Nodes);

	FImportedModel ImportedModel;

	// Import transformed meshes starting from nodes.
	for (size_t i = 0; i < Nodes.Size(); i++)
	{
		FAssimpNode& Node = Nodes[i];

		for (size_t k = 0; k < Node.Node->mNumMeshes; k++)
		{
			int MeshIndex = Node.Node->mMeshes[k];
			CreateMeshData(Node.Node, Scene->mMeshes[MeshIndex], ImportedModel);
		}
	}

	FModelData ModelData;
	ModelData.VertexCount = ImportedModel.Vertices.Size();
	ModelData.ChannelFlag = EMeshChannelType::DiffuseUV | EMeshChannelType::Position | EMeshChannelType::Normal | EMeshChannelType::Tangent | EMeshChannelType::BiTangent;
	ModelData.Indices = ImportedModel.Indices;
	ModelData.ResizeChannels(ImportedModel.Vertices.Size());

	for (size_t i = 0; i < Scene->mNumMaterials; i++)
	{
		FMaterialData MaterialData;

		aiMaterial* Material = Scene->mMaterials[i];

		int TextureCount = Material->GetTextureCount(aiTextureType_UNKNOWN);

		ImportTexture(Material, aiTextureType_DIFFUSE, "Albedo", MaterialData.DiffuseTexturePath);
		ImportTexture(Material, aiTextureType_HEIGHT, "Normal", MaterialData.NormalTexturePath);
		ImportTexture(Material, aiTextureType_AMBIENT, "Reflection", MaterialData.RefractionTexturePath);
		ImportTexture(Material, aiTextureType_SHININESS, "Roughness", MaterialData.RoughnessTexturePath);
		ImportTexture(Material, aiTextureType_OPACITY, "Opacity", MaterialData.OpacityTexturePath);

		aiColor3D Colour;
		if (Material->Get(AI_MATKEY_COLOR_DIFFUSE, Colour) == aiReturn_SUCCESS)
			MaterialData.DiffuseColour = ToGlmVector(Colour);
		else if (Material->Get(AI_MATKEY_COLOR_AMBIENT, Colour) == aiReturn_SUCCESS)
			MaterialData.DiffuseColour = ToGlmVector(Colour);

		if (Material->Get(AI_MATKEY_COLOR_SPECULAR, Colour) == aiReturn_SUCCESS)
			MaterialData.SpecularColour = ToGlmVector(Colour);

		float Roughness;
		if (Material->Get(AI_MATKEY_SHININESS, Roughness) == aiReturn_SUCCESS)
			MaterialData.Roughness = Roughness;

		float Refraction;
		if (Material->Get(AI_MATKEY_REFRACTI, Refraction) == aiReturn_SUCCESS)
			MaterialData.Refraction = Refraction;

		ModelData.Materials.Add(MaterialData);
	}

	for (auto MeshPart : ImportedModel.Meshes)
	{
		FMeshData MeshData;
		MeshData.IndexStart = MeshPart.IndexStart;
		MeshData.IndexCount = MeshPart.IndexCount;
		MeshData.MaterialIndex = MeshPart.MaterialIndex;
		MeshData.BaseVertex = MeshPart.BaseVertex;
		ModelData.MeshData.Add(MeshData);
	}

	for (int i = 0; i < ModelData.VertexCount; i++)
	{
		ModelData.PositionChannel.Elements[i] = ImportedModel.Vertices[i].Position;
		ModelData.DiffuseUVChannel.Elements[i] = ImportedModel.Vertices[i].UV;
		ModelData.NormalChannel.Elements[i] = ImportedModel.Vertices[i].Normal;
		ModelData.TangentChannel.Elements[i] = ImportedModel.Vertices[i].Tangent;
		ModelData.BiTangentChannel.Elements[i] = ImportedModel.Vertices[i].BiTangent;
	}

	// Import skeleton structure

	ModelData.Serialize(Serializer);

	return true;
}