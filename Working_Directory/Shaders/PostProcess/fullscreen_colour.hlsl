#include "fullscreen_fast.hlsl"

Texture2D ScreenTexture;

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	return ScreenTexture.Sample(SimpleSampler, Input.Texcoord);
}
