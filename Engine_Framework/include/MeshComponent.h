#ifndef MeshComponent_h__
#define MeshComponent_h__

#include "Component.h"
#include "Transform.h"

class FMesh;

class FMeshComponent : public TComponent<FMeshComponent>
{
public:
	FMeshComponent(class FMesh* Mesh, bool bTessellatedMesh = false);
	~FMeshComponent();

	class FMesh* Mesh;
	bool bTessellatedMesh;
	bool bVisible = true;
};

class FTransformComponent : public TComponent<FTransformComponent>
{
public:
	FTransformComponent(const FTransform& Transform) : Transform(Transform) { }
	FTransform Transform;
};

#endif // MeshComponent_h__
