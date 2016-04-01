#include "buffers.hlsl"
#include "Shading/normal_mapping.hlsl"

struct Vertex_Input
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	float2 Texcoord : TEXCOORD;
};

struct Vertex_Output
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	float2 Texcoord : TEXCOORD;
	float2 Depthcoord : TEXCOORD1;
};

struct Pixel_Output
{
	float4 Normal : SV_TARGET0;
	float Depth : SV_TARGET1;
};

// Vertex shader code
Vertex_Output Vertex(Vertex_Input Input)
{
	Vertex_Output Output;

	Output.Position = mul(float4(Input.Position, 1.0), WorldMatrix);
	Output.Position = mul(Output.Position, ViewMatrix);
	Output.Position = mul(Output.Position, ProjectionMatrix);
	Output.Normal = normalize(mul(float4(Input.Normal, 1.0), InverseTransposeWorldMatrix)).xyz;
	Output.Tangent = normalize(mul(float4(Input.Tangent, 1.0), InverseTransposeWorldMatrix)).xyz;
	Output.BiTangent = normalize(mul(float4(Input.BiTangent, 1.0), InverseTransposeWorldMatrix)).xyz;
	Output.Texcoord = Input.Texcoord;
	Output.Depthcoord = Output.Position.zw;

	return Output;
}

Pixel_Output Pixel(Vertex_Output Input)
{
	Pixel_Output Output;

	float4 Normal = mul(float4(DoNormalMapping(Input.Texcoord, Input.Normal, Input.Tangent, Input.BiTangent), 1.0), InverseTransposeViewMatrix);
	Output.Normal = Normal;
	Output.Depth = Input.Depthcoord.x / Input.Depthcoord.y;

	return Output;
}
