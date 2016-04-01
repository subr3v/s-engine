Texture2D DiffuseTexture;

#include "fullscreen_fast.hlsl"

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	return DiffuseTexture.Sample(LinearSampler, Input.Texcoord);
}
