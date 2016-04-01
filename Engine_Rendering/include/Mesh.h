#ifndef Mesh_h__
#define Mesh_h__

#include "Vector.h"
#include <glm/glm.hpp>
#include <d3d11.h>
#include "DrawCallParameters.h"
#include "Core.hpp"
#include "MaterialParameters.h"
#include "GraphicsBuffer.h"
#include "UniquePtr.h"

struct FWeightIndexPair
{
	float Weight;
	int BoneIndex;

	FWeightIndexPair() { }
	FWeightIndexPair(float Weight, int BoneIndex) : Weight(Weight), BoneIndex(BoneIndex) { }
};

struct FSkinningData
{
	TVector<FWeightIndexPair> BoneInfluences;
};

struct FMeshPartData
{
	FMeshPartData(int StartIndex, int IndexCount, int MaterialIndex, int BaseVertex) : StartIndex(StartIndex), IndexCount(IndexCount), MaterialIndex(MaterialIndex), BaseVertex(BaseVertex) { }

	int GetStartIndex() const { return StartIndex; }
	int GetIndexCount() const { return IndexCount; }
	int GetMaterialIndex() const { return MaterialIndex; }
	int GetBaseVertex() const { return BaseVertex; }
private:
	int StartIndex;
	int IndexCount;
	int MaterialIndex;
	int BaseVertex;
};

struct FMeshMaterialParameters
{
	class FMaterial* Material = nullptr;
	class FTexture* DiffuseTexture = nullptr;
	class FTexture* OpacityTexture = nullptr;
	class FTexture* NormalTexture = nullptr;
	class FTexture* RoughnessTexture = nullptr;
	class FTexture* RefractionTexture = nullptr;
	glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	float Roughness = 1.0;
	float RefractionIndex = 1.0;

	FMeshMaterialParameters() { }
	FMeshMaterialParameters(FMaterial* Material, FTexture* DiffuseTexture = nullptr) : Material(Material), DiffuseTexture(DiffuseTexture) { }

	void Apply(ID3D11DeviceContext* DeviceContext, FMaterialParameters* Parameters) const;
};

struct FMeshBuilder
{
	FMeshBuilder(int VertexSize, int VertexCount = 0, int IndexCount = 0);

	void AddIndex(u32 Index);
	void CopyIndices(const TVector<u32>& Indices);
	void GenerateIndices(s32 StartIndex, u32 Count);

	void AddPart(const FMeshPartData& MeshPartData);
	void AddMaterialParameters(const FMeshMaterialParameters& MeshParameters);

	template <typename T>
	void CopyAddVertex(T VertexData)
	{
		assert(sizeof(T) == VertexSize);
		AddVertex(&VertexData);
	}

	int GetVertexSize() const { return VertexSize; }
	int GetVertexCount() const { return VerticesData.Size() / VertexSize; }

	const TVector<FMeshMaterialParameters>& GetMeshMaterials() const { return MeshMaterials; }
	const TVector<FMeshPartData>& GetMeshParts() const { return MeshParts; }
	const TVector<u32>& GetIndices() const { return IndicesData; }
	const TVector<u8>& GetVertices() const { return VerticesData; }
private:
	void AddVertex(void* Data);

	u32 CurrentIndex;
	u32 CurrentVertex;
	u32 VertexSize;
	TVector<u8> VerticesData;
	TVector<u32> IndicesData;

	TVector<FMeshMaterialParameters> MeshMaterials;
	TVector<FMeshPartData> MeshParts;
};

struct FMeshPart
{
	FMeshPart(const FMeshPartData& Data)
	{
		this->IndexStart = Data.GetStartIndex();
		this->IndexCount = Data.GetIndexCount();
		this->MaterialIndex = Data.GetMaterialIndex();
		this->BaseVertex = Data.GetBaseVertex();
	}

	int IndexStart;
	int IndexCount;
	int BaseVertex;
	int MaterialIndex;
};

// Represent a mesh that has been submitted to the GPU.
class FMesh
{
public:
	FMesh(class FGraphicsContext* GraphicsContext, const FMeshBuilder& MeshBuilder);
	~FMesh();

	void Build(class FGraphicsContext* GraphicsContext, const FMeshBuilder& MeshBuilder);
	void BindBuffers(ID3D11DeviceContext* DeviceContext) const;

	const FMeshMaterialParameters& GetParameters(int Index) const { return MaterialParameters[Index]; }
	const TVector<FMeshPart>& GetParts() const { return Parts; }
private:
	TVector<FMeshPart> Parts;
	TVector<FMeshMaterialParameters> MaterialParameters;

	TUniquePtr<FGraphicsBuffer> VertexBuffer;
	TUniquePtr<FGraphicsBuffer> IndexBuffer;

	size_t IndexCount;
	size_t VertexCount;
	size_t VertexSize;
};

#endif // Mesh_h__
