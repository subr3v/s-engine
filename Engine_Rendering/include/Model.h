#ifndef ModelData_h__
#define ModelData_h__

#include <map>
#include "Vector.h"
#include <glm\glm.hpp>

class FGraphicsContext;

struct FModelVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
	glm::vec2 UV;

	FModelVertex() { }
	FModelVertex(const glm::vec3& Position, const glm::vec3& Normal, const glm::vec2& UV) : Position(Position), Normal(Normal), UV(UV) { }
};

struct FModelSkinnedVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
	glm::vec4 BoneWeights;
	glm::ivec4 BoneIndices;
};

struct FModel
{
	FModel();
	~FModel();

	class FSkeleton* Skeleton = nullptr;
	class FMesh* Mesh = nullptr;
	TVector<class FSkeletalAnimation*> Animations;
};

#endif // ModelData_h__
