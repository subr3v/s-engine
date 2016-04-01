Texture2D DiffuseTexture;
Texture2D NormalBuffer;

#include "fullscreen_fast.hlsl"

cbuffer AmbientOcclusionBuffer
{
	float4 AmbientKernel[16];
	float AmbientKernelSize; float Radius; float DistanceThreshold; float Padding;
};

float Pixel(Vertex_Output Input) : SV_TARGET
{
	float Depth = DiffuseTexture.Sample(SimpleSampler, Input.Texcoord).r;
	float3 ViewPosition = VSPositionFromDepth(Depth, Input.Texcoord);
	float3 Normal = NormalBuffer.Sample(SimpleSampler, Input.Texcoord).xyz;
	
	float AccumulationBuffer = 0.0;
	float PreWeight = 1.0 / AmbientKernelSize;

	for (int i = 0; i < 16; i++)
	{
		float2 SampleCoordinate = Input.Texcoord + AmbientKernel[i].xy * Radius;
		float SampleDepth = DiffuseTexture.Sample(SimpleSampler, SampleCoordinate).r;
		float3 SampleViewPosition = VSPositionFromDepth(SampleDepth, SampleCoordinate);
		float3 SampleDirection = normalize(SampleViewPosition - ViewPosition);

		// angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
		float NdotS = max(dot(Normal, SampleDirection), 0);
		// distance between SURFACE-POSITION and SAMPLE-POSITION
		float VPdistSP = distance(ViewPosition, SampleViewPosition);

		// a = distance function
		float a = 1.0 - smoothstep(DistanceThreshold, DistanceThreshold * 2, VPdistSP);
		// b = dot-Product
		float b = NdotS;

		AccumulationBuffer += (a * b) * PreWeight;
	}

	AccumulationBuffer = (1 - AccumulationBuffer);

	return AccumulationBuffer;
}
