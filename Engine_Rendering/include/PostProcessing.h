#ifndef PostProcessing_h__
#define PostProcessing_h__

#include <d3d11.h>
#include "Viewport.h"

class FPostProcessing
{
public:
	static void Apply(class FGraphicsContext*  GraphicsContext, const FViewport& CurrentViewport, ID3D11ShaderResourceView* Source, ID3D11RenderTargetView* Target, class FMaterial* Material, float ViewportWidth = 0.0f, float ViewportHeight = 0.0f);
};

#endif // PostProcessing_h__
