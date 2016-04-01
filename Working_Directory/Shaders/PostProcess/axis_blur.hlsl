Texture2D DiffuseTexture;

#include "fullscreen_fast.hlsl"

cbuffer BlurBuffer
{
	float2 Blur_TextureOffset;
	float2 Blur_Padding;
};

static const float kBlurKernel0 = 0.000003;
static const float kBlurKernel1 = 0.000229;
static const float kBlurKernel2 = 0.005977;
static const float kBlurKernel3 = 0.060598;
static const float kBlurKernel4 = 0.24173;
static const float kBlurKernel5 = 0.382925;
static const float kBlurKernel6 = 0.24173;
static const float kBlurKernel7 = 0.060598;
static const float kBlurKernel8 = 0.005977;
static const float kBlurKernel9 = 0.000229;
static const float kBlurKernel10 = 0.000003;

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float4 Value = DiffuseTexture.Sample(LinearSampler, Input.Texcoord - Blur_TextureOffset * 5) * kBlurKernel0;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord - Blur_TextureOffset * 4) * kBlurKernel1;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord - Blur_TextureOffset * 4) * kBlurKernel2;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord - Blur_TextureOffset * 2) * kBlurKernel3;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord - Blur_TextureOffset * 1) * kBlurKernel4;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord) * kBlurKernel5;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord + Blur_TextureOffset * 1) * kBlurKernel6;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord + Blur_TextureOffset * 2) * kBlurKernel7;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord + Blur_TextureOffset * 3) * kBlurKernel8;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord + Blur_TextureOffset * 4) * kBlurKernel9;
	Value += DiffuseTexture.Sample(LinearSampler, Input.Texcoord + Blur_TextureOffset * 5) * kBlurKernel10;

	return Value;
}
