#include "Mesh.h"
#include "Material.h"
#include "GraphicsContext.h"
#include "Texture.h"
#include <algorithm>
#include "DrawCallParameters.h"
#include "GraphicsBuffer.h"
#include "ConstantBufferTypes.h"

FMesh::FMesh(FGraphicsContext* GraphicsContext, const FMeshBuilder& MeshBuilder)
{
	Build(GraphicsContext, MeshBuilder);
}

void FMesh::Build(class FGraphicsContext* GraphicsContext, const FMeshBuilder& MeshBuilder)
{
	Parts.Clear();
	MaterialParameters.Clear();

	// Copy parts data
	for (auto PartData : MeshBuilder.GetMeshParts())
	{
		Parts.Add(FMeshPart(PartData));
	}

	MaterialParameters = MeshBuilder.GetMeshMaterials();

	// We want to sort parts based on their material index to speed up rendering (avoiding state changes).
	std::sort(Parts.begin(), Parts.end(),
		[](const FMeshPart& A, const FMeshPart& B) { return A.MaterialIndex > B.MaterialIndex; });

	ID3D11Device* Device = GraphicsContext->GetDevice();

	IndexCount = MeshBuilder.GetIndices().Size();
	VertexCount = MeshBuilder.GetVertexCount();
	VertexSize = MeshBuilder.GetVertexSize();

	VertexBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, EBufferType::Vertex, EBufferUsage::Static, VertexCount * MeshBuilder.GetVertexSize(), MeshBuilder.GetVertices().Data());
	IndexBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, EBufferType::Index, EBufferUsage::Static, IndexCount * sizeof(uint32_t), MeshBuilder.GetIndices().Data());
}

FMesh::~FMesh()
{

}

void FMesh::BindBuffers(ID3D11DeviceContext* DeviceContext) const
{
	UINT Offsets = 0;
	ID3D11Buffer* Vb = VertexBuffer->GetBuffer();
	DeviceContext->IASetVertexBuffers(0, 1, &Vb, &VertexSize, &Offsets);
	DeviceContext->IASetIndexBuffer(IndexBuffer->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

FMeshBuilder::FMeshBuilder(int VertexSize, int VertexCount, int IndexCount) : VertexSize(VertexSize), CurrentIndex(0), CurrentVertex(0)
{
	VerticesData.Reserve(VertexCount * VertexSize);
	IndicesData.Reserve(IndexCount);
}

void FMeshBuilder::AddVertex(void* Data)
{
	if (VerticesData.Size() < (CurrentVertex + 1) * VertexSize)
	{
		VerticesData.Resize(VerticesData.Size() + VertexSize);
	}
	
	unsigned char* DestinationAddress = VerticesData.Data() + (VertexSize * CurrentVertex);
	memcpy(DestinationAddress, Data, VertexSize);
	CurrentVertex++;
}

void FMeshBuilder::AddIndex(u32 Index)
{
	IndicesData.Add(Index);
}

void FMeshBuilder::AddPart(const FMeshPartData& MeshPartData)
{
	MeshParts.Add(MeshPartData);
}

void FMeshBuilder::AddMaterialParameters(const FMeshMaterialParameters& MeshParameters)
{
	MeshMaterials.Add(MeshParameters);
}

void FMeshBuilder::CopyIndices(const TVector<u32>& Indices)
{
	IndicesData = Indices;
}

void FMeshBuilder::GenerateIndices(s32 StartIndex, u32 Count)
{
	for (u32 i = StartIndex; i < StartIndex + Count; i++)
	{
		AddIndex(i);
	}
}

void FMeshMaterialParameters::Apply(ID3D11DeviceContext* DeviceContext, FMaterialParameters* Parameters) const
{
	if (DiffuseTexture != nullptr && DiffuseTexture->GetResourceView() != nullptr)
		Parameters->SetResource(FTextureNames::kDiffuseMap, DiffuseTexture->GetResourceView());

	if (NormalTexture != nullptr && NormalTexture->GetResourceView() != nullptr)
		Parameters->SetResource(FTextureNames::kNormalMap, NormalTexture->GetResourceView());

	if (RoughnessTexture != nullptr && RoughnessTexture->GetResourceView() != nullptr)
		Parameters->SetResource(FTextureNames::kRoughnessMap, RoughnessTexture->GetResourceView());

	if (RefractionTexture != nullptr && RefractionTexture->GetResourceView() != nullptr)
		Parameters->SetResource(FTextureNames::kRefractionMap, RefractionTexture->GetResourceView());

	//TODO: Create an unique allocated buffer so that uploads are 16 bytes aligned.
	FMaterialBuffer Buffer;
	Buffer.Albedo = Albedo;
	Buffer.Roughness = Roughness;
	Buffer.RefractionIndex = RefractionIndex;

	Parameters->UpdateUniform(DeviceContext, FMaterialBuffer::kBufferName, &Buffer, sizeof(FMaterialBuffer));
}
