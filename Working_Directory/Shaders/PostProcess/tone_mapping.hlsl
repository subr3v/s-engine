Texture2D OriginalTexture;
Texture2D BloomTexture;
Texture2D LuminanceTexture;

#include "fullscreen_fast.hlsl"

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
}

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float3 Original = OriginalTexture.Sample(SimpleSampler, Input.Texcoord).rgb;
	float3 Bloom = BloomTexture.Sample(TextureSampler, Input.Texcoord).rgb;
	float3 Colour = Original + Bloom * BloomStrength;

	float Exposure = 1.0 / LuminanceTexture.Load(uint3(0, 0, 0)).r;
	Colour *= Exposure;

	float ExposureBias = 2.0f;
	float3 Current = Uncharted2Tonemap(ExposureBias * Colour);
	float3 WhiteScale = 1.0f / Uncharted2Tonemap(W);
	Colour = Current * WhiteScale;
	return float4(pow(Colour, 1.0 / 2.2), 1.0);
}
