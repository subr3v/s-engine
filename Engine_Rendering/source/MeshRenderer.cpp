#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "ConstantBufferTypes.h"

void FMeshRenderer::Render(const FMesh* Mesh, const FDrawCall& DrawCallParameters, bool bUseTessellation)
{
	assert(Mesh);

	FProfiler::Get().ProfileCounter("Rendering", "Mesh Draw Call", true, 1);

	ID3D11DeviceContext* DeviceContext = DrawCallParameters.DeviceContext;

	DeviceContext->IASetPrimitiveTopology(bUseTessellation ? D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Mesh->BindBuffers(DeviceContext);

	int PreviousMaterialIndex = -1;

	FMaterial* CurrentMaterial = nullptr;

	if (DrawCallParameters.OverrideMaterial == false && DrawCallParameters.StandardMaterial == nullptr)
	{
		for (FMeshPart Part : Mesh->GetParts())
		{
			if (PreviousMaterialIndex != Part.MaterialIndex)
			{
				PreviousMaterialIndex = Part.MaterialIndex;
				const FMeshMaterialParameters& ParametersData = Mesh->GetParameters(Part.MaterialIndex);

				CurrentMaterial = ParametersData.Material;

				FMaterialParameters* Parameters = CurrentMaterial->GetParameters();
				ParametersData.Apply(DeviceContext, Parameters);
				Parameters->UpdateUniform(DeviceContext, FInstanceBuffer::kBufferName, (void*)DrawCallParameters.InstanceData, sizeof(FInstanceBuffer));

				FProfiler::Get().ProfileCounter("Rendering", "Mesh Material State Change", true, 1);
				CurrentMaterial->Apply(DeviceContext);
			}

			DrawCallParameters.DrawIndexed(Part.IndexCount, Part.IndexStart, Part.BaseVertex);
		}
	}
	else if (DrawCallParameters.OverrideMaterial)
	{
		for (FMeshPart Part : Mesh->GetParts())
		{
			if (PreviousMaterialIndex != Part.MaterialIndex)
			{
				PreviousMaterialIndex = Part.MaterialIndex;
				const FMeshMaterialParameters& ParametersData = Mesh->GetParameters(Part.MaterialIndex);

				CurrentMaterial = DrawCallParameters.OverrideMaterial(ParametersData.Material);

				FMaterialParameters* Parameters = CurrentMaterial->GetParameters();
				ParametersData.Apply(DeviceContext, Parameters);
				Parameters->UpdateUniform(DeviceContext, FInstanceBuffer::kBufferName, (void*)DrawCallParameters.InstanceData, sizeof(FInstanceBuffer));

				FProfiler::Get().ProfileCounter("Rendering", "Mesh Material State Change", true, 1);
				CurrentMaterial->Apply(DeviceContext);
			}

			DrawCallParameters.DrawIndexed(Part.IndexCount, Part.IndexStart, Part.BaseVertex);
		}
	}
	else if (DrawCallParameters.StandardMaterial)
	{
		for (FMeshPart Part : Mesh->GetParts())
		{
			if (PreviousMaterialIndex != Part.MaterialIndex)
			{
				PreviousMaterialIndex = Part.MaterialIndex;
				const FMeshMaterialParameters& ParametersData = Mesh->GetParameters(Part.MaterialIndex);

				CurrentMaterial = DrawCallParameters.StandardMaterial;

				FMaterialParameters* Parameters = CurrentMaterial->GetParameters();
				ParametersData.Apply(DeviceContext, Parameters);
				Parameters->UpdateUniform(DeviceContext, FInstanceBuffer::kBufferName, (void*)DrawCallParameters.InstanceData, sizeof(FInstanceBuffer));

				FProfiler::Get().ProfileCounter("Rendering", "Mesh Material State Change", true, 1);
				CurrentMaterial->Apply(DeviceContext);
			}

			DrawCallParameters.DrawIndexed(Part.IndexCount, Part.IndexStart, Part.BaseVertex);
		}
	}
}


