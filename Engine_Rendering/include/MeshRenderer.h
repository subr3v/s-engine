#ifndef StaticModel_h__
#define StaticModel_h__

#include "Mesh.h"
#include "DrawCallParameters.h"

class FMeshRenderer
{
public:
	static void Render(const FMesh* Mesh, const FDrawCall& DrawCallParameters, bool bUseTessellation);
private:
};


#endif // StaticModel_h__
