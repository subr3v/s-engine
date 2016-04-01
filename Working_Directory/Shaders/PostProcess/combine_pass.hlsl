Texture2D DiffuseTexture;

#include "fullscreen_fast.hlsl"

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float3 FinalColour = 0.5 * DiffuseTexture.Sample(SimpleSampler, Input.Texcoord).rgb + 1.0;

#if defined(GAMMA_CORRECTION)
	//FinalColour = pow(FinalColour, 1 / 2.2);
#endif

	return float4(FinalColour, 1.0);
}
