#include "MeshComponent.h"
#include "MeshRenderer.h"

FMeshComponent::FMeshComponent(FMesh* Mesh, bool bTessellatedMesh) : Mesh(Mesh), bTessellatedMesh(bTessellatedMesh)
{
}

FMeshComponent::~FMeshComponent()
{
}

/*
void FMeshComponent::Render(FDrawCall DrawCallParameters)
{
	FMeshRenderer::Render(Mesh, DrawCallParameters, bTessellatedMesh);
}
*/