#ifndef Renderable_h__
#define Renderable_h__

#include <glm/glm.hpp>
#include <d3d11.h>
#include <functional>

struct FInstanceBuffer;
class FMaterial;

struct FDrawCall
{
	ID3D11DeviceContext* DeviceContext = nullptr;
	FInstanceBuffer* InstanceData;
	FMaterial* StandardMaterial = nullptr;
	std::function<FMaterial*(FMaterial*)> OverrideMaterial;

	FDrawCall(ID3D11DeviceContext* DeviceContext, FInstanceBuffer* InstanceData, FMaterial* StandardMaterial = nullptr)
		: DeviceContext(DeviceContext), InstanceData(InstanceData), StandardMaterial(StandardMaterial) { }
	
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) const;
	void Draw(UINT VertexCount, UINT StartVertexLocation) const;
};

#endif // Renderable_h__
