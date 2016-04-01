#ifndef FullscreenQuad_h__
#define FullscreenQuad_h__

#include "Mesh.h"
#include "UniquePtr.h"

class FFullScreenQuad
{
public:
	FFullScreenQuad(FGraphicsContext* GraphicsContext, FMaterial* DefaultMaterial);
	~FFullScreenQuad();

	const FMesh* GetMesh() const { return QuadMesh.Get(); }
private:
	TUniquePtr<FMesh> QuadMesh;
};


#endif // FullscreenQuad_h__
