#ifndef RenderState_h__
#define RenderState_h__

#include <d3d11.h>
#include "UniquePtr.h"

class FRenderStates
{
public:
	FRenderStates(class FGraphicsContext* GraphicsContext);
	~FRenderStates();

	ID3D11RasterizerState* SolidRasterizer;
	ID3D11RasterizerState* SolidNoCullRasterizer;
	ID3D11RasterizerState* WireframeRasterizer;

	ID3D11DepthStencilState* LessDepthState;
	ID3D11DepthStencilState* NoDepthState;

	ID3D11BlendState* AlphaBlendState;
	ID3D11BlendState* PremultipliedAlphaBlendState;
	ID3D11BlendState* OpaqueBlendState;
	ID3D11BlendState* AdditiveBlendState;

	ID3D11SamplerState* SimpleSampler;
	ID3D11SamplerState* LinearSampler;
	ID3D11SamplerState* TextureSampler;
};

#endif // RenderState_h__
