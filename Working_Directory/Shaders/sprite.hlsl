#include "buffers.hlsl"

Texture2D DiffuseTexture;

struct Vertex_Input
{
	float2 Position : POSITION;
	float2 Texcoord : TEXCOORD;
	float4 Colour : COLOR0;
};

struct Vertex_Output
{
	float4 Position : SV_POSITION;
	float4 Colour : COLOR0;
	float2 Texcoord : TEXCOORD;
};

// Vertex shader code
Vertex_Output Vertex(Vertex_Input Input)
{
	Vertex_Output Output;

	Output.Position = mul(float4(Input.Position.xy, 0.0, 1.0), WorldMatrix);
	Output.Position = mul(float4(Output.Position), SpriteProjectionMatrix);
	Output.Colour = Input.Colour;
	Output.Texcoord = Input.Texcoord;

	return Output;
}

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float4 Diffuse = DiffuseTexture.Sample(TextureSampler, Input.Texcoord);
	return Diffuse * Input.Colour;
}
