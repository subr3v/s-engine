Texture2D DiffuseTexture;

#include "fullscreen_fast.hlsl"

static const float3 LUM_CONVERT = float3(0.299f, 0.587f, 0.114f);

float Pixel(Vertex_Output Input) : SV_TARGET
{
	float Luminance = max(dot(DiffuseTexture.Sample(TextureSampler, Input.Texcoord).rgb, LUM_CONVERT), 0.00001f);
	return (Luminance);
}
