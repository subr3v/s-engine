#include "buffers.hlsl"

Texture2D DiffuseTexture;

struct Vertex_Input
{
	float2 Position : POSITION;
	float2 Size : SIZE;
	float Rotation : ROTATION;
	float2 Origin : ORIGIN;
	float2 UvCoordinate : TEXCOORD0;
	float2 UvSize : TEXCOORD1;
	float4 Colour : COLOR0;
};

struct Vertex_Output
{
	float4 Position : SV_POSITION;
	float4 Colour : COLOR0;
	float2 Texcoord : TEXCOORD;
};

struct Pixel_Input
{
	float4 Position : SV_POSITION;
	float4 Colour : COLOR0;
	float2 Texcoord : TEXCOORD;
};

// Vertex shader code
Vertex_Input Vertex(Vertex_Input Input)
{
	Vertex_Input Output = Input;

	Output.Position = mul(float4(Input.Position.xy, 0.0, 1.0), WorldMatrix).xy;

	return Output;
}

float2 RotatePoint(float2 Point, float Angle)
{
	float Cosine = cos(Angle);
	float Sine = sin(Angle);
	return float2(Point.x * Cosine - Point.y * Sine, Point.x * Sine + Point.y * Cosine);
}

[maxvertexcount(4)]
void Geometry(point Vertex_Input SpritePoint[1], inout TriangleStream<Pixel_Input> TriStream)
{
	Pixel_Input Vertex;
	
	float2 P1 = RotatePoint(-SpritePoint[0].Origin, SpritePoint[0].Rotation);
	float2 P2 = RotatePoint(-SpritePoint[0].Origin + float2(SpritePoint[0].Size.x, 0.0), SpritePoint[0].Rotation);
	float2 P3 = RotatePoint(-SpritePoint[0].Origin + float2(SpritePoint[0].Size.x, SpritePoint[0].Size.y), SpritePoint[0].Rotation);
	float2 P4 = RotatePoint(-SpritePoint[0].Origin + float2(0.0, SpritePoint[0].Size.y), SpritePoint[0].Rotation);
	
	Vertex.Position = mul(float4(SpritePoint[0].Position + P1, 0.0, 1.0), SpriteProjectionMatrix);
	Vertex.Colour = SpritePoint[0].Colour;
	Vertex.Texcoord = SpritePoint[0].UvCoordinate;

	TriStream.Append(Vertex);

	Vertex.Position = mul(float4(SpritePoint[0].Position + P2, 0.0, 1.0), SpriteProjectionMatrix);
	Vertex.Texcoord = SpritePoint[0].UvCoordinate + float2(SpritePoint[0].UvSize.x, 0.0);

	TriStream.Append(Vertex);

	Vertex.Position = mul(float4(SpritePoint[0].Position + P4, 0.0, 1.0), SpriteProjectionMatrix);
	Vertex.Texcoord = SpritePoint[0].UvCoordinate + float2(0.0, SpritePoint[0].UvSize.y);

	TriStream.Append(Vertex);

	Vertex.Position = mul(float4(SpritePoint[0].Position + P3, 0.0, 1.0), SpriteProjectionMatrix);
	Vertex.Texcoord = SpritePoint[0].UvCoordinate + float2(SpritePoint[0].UvSize.x, SpritePoint[0].UvSize.y);

	TriStream.Append(Vertex);
}

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float4 Diffuse = DiffuseTexture.Sample(SimpleSampler, Input.Texcoord);
	return Diffuse * Input.Colour;
}
