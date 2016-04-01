#ifndef PrimitiveBuilder_h__
#define PrimitiveBuilder_h__

#include "glm/glm.hpp"
#include "Mesh.h"

class FGraphicsContext;
class FMesh;

class FMeshUtility
{
public:
	// Generates a plane.
	static FMeshBuilder GeneratePlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize);
	
	static FMeshBuilder GenerateTerrainQuiltPlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize);

	static FMeshBuilder GenerateTerrainQuilt(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize, float MaxHeight, const TVector<float>& Heightmap);

	// Generates a plane with control points
	static FMeshBuilder GenerateTessellationPlane(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform, int Width, int Height, float CellSize);

	// Generates a unit cube with the specified transform and colour.
	static FMeshBuilder GenerateCube(FMeshMaterialParameters MaterialParameters, const glm::mat4& Transform);
	
	// Generates a unit sphere with the specified transform and colour.
	static FMeshBuilder GenerateSphere(FMeshMaterialParameters MaterialParameters, int SubdivisionNumber, const glm::mat4& Transform);
};

#endif // PrimitiveBuilder_h__
