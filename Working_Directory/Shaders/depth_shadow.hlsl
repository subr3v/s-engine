#include "buffers.hlsl"

cbuffer ShadowInstanceBuffer
{
	int CurrentShadowMap; float3 Padding;
};

struct Vertex_Input
{
	float3 Position : POSITION;
};

struct Vertex_Output
{
	float4 Position : SV_POSITION;
	float2 Depth : TEXCOORD1;
};

Texture2D DiffuseTexture;

// Vertex shader code
Vertex_Output Vertex(Vertex_Input Input)
{
	Vertex_Output Output;

	Output.Position = mul(float4(Input.Position, 1.0), WorldMatrix);
	Output.Position = mul(Output.Position, LightViewProjectionMatrix[CurrentShadowMap]);
	Output.Depth = Output.Position.zw;

	return Output;
}

float Pixel(Vertex_Output Input) : SV_TARGET
{
	float Depth = Input.Depth.x / Input.Depth.y;
	return Depth;
}
