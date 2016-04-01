#include "FullscreenQuad.h"

struct FVertexData
{
	glm::vec3 Position;
	glm::vec2 Texcoord;

	FVertexData(const glm::vec3& Position, const glm::vec2& Texcoord) : Position(Position), Texcoord(Texcoord) { }
};

FFullScreenQuad::FFullScreenQuad(FGraphicsContext* GraphicsContext, FMaterial* DefaultMaterial)
{
	FMeshMaterialParameters Params;
	Params.DiffuseTexture = nullptr;
	Params.NormalTexture = nullptr;
	Params.Material = DefaultMaterial;

	FMeshBuilder QuadData(sizeof(FVertexData), 4, 6);

	QuadData.CopyAddVertex(FVertexData(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	QuadData.CopyAddVertex(FVertexData(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	QuadData.CopyAddVertex(FVertexData(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	QuadData.CopyAddVertex(FVertexData(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));

	QuadData.CopyIndices({ 0, 1, 2, 0, 2, 3 });
	
	QuadData.AddMaterialParameters(Params);
	QuadData.AddPart(FMeshPartData(0, 6, 0, 0));

	QuadMesh = Make_Unique<FMesh>(GraphicsContext, QuadData);
}

FFullScreenQuad::~FFullScreenQuad()
{
}
