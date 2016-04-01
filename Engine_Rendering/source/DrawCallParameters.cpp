#include "DrawCallParameters.h"

void FDrawCall::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) const
{
	DeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void FDrawCall::Draw(UINT VertexCount, UINT StartVertexLocation) const
{
	DeviceContext->Draw(VertexCount, StartVertexLocation);
}
