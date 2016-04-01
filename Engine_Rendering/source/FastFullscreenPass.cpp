#include "FastFullscreenPass.h"
#include "Material.h"

void FFastFullscreenPass::Render(const FDrawCall& DrawCallParameters)
{
	ID3D11DeviceContext* DeviceContext = DrawCallParameters.DeviceContext;

	DeviceContext->IASetInputLayout(nullptr);

	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* VBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] { nullptr };
	UINT Offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] { 0 };

	DeviceContext->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, VBuffers, Offsets, Offsets);
	DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

	DrawCallParameters.StandardMaterial->Apply(DeviceContext);

	DeviceContext->Draw(3, 0);
}
