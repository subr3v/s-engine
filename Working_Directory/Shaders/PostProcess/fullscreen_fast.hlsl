#include "../buffers.hlsl"

struct Vertex_Output
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD;
};

Vertex_Output Vertex(uint id: SV_VertexID)
{
	Vertex_Output Output;

	id = 2 - id;

	Output.Position.x = (float)(id / 2) * 4.0 - 1.0;
	Output.Position.y = (float)(id % 2) * 4.0 - 1.0;
	Output.Position.z = 0.0;
	Output.Position.w = 1.0;

	Output.Texcoord.x = (float)(id / 2) * 2.0;
	Output.Texcoord.y = 1.0 - (float)(id % 2) * 2.0;

	return Output;
}