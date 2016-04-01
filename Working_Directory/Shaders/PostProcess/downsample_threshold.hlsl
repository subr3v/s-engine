Texture2D DiffuseTexture;
Texture2D LuminanceTexture;

#include "fullscreen_fast.hlsl"

static const float3 LUM_CONVERT = float3(0.299f, 0.587f, 0.114f);

cbuffer BloomSettings
{
	float LuminanceThreshold;
};

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float AverageLuminance = LuminanceTexture.Load(uint3(0, 0, 0)).r;
	float4 Colour = DiffuseTexture.Sample(SimpleSampler, Input.Texcoord);
	float Luminance = max(dot(Colour.rgb, LUM_CONVERT), 0.000001f);
	if ((Luminance / (AverageLuminance)) >= LuminanceThreshold)
	{
		if (dot(Colour, 0.333f) <= 0.01f)
			Colour = 0.0f;
		return Colour;
	}
	else
		return float4(0.0, 0.0, 0.0, 1.0);
}
