cbuffer ObjectBuffer
{
	row_major matrix WorldMatrix;
	row_major matrix InverseTransposeWorldMatrix;
};

cbuffer FrameBuffer
{
	row_major matrix ViewMatrix;
	row_major matrix ProjectionMatrix;
	row_major matrix InverseViewMatrix;
	row_major matrix InverseProjectionMatrix;
	row_major matrix InverseTransposeViewMatrix;
	row_major matrix SpriteProjectionMatrix;
	float3 CameraPosition; uint NumLights; // 0xFFFF0000 has point light num encoded and 0x0000FFFF has spot light num encoded
	float TotalTime; float FrameDelta; float2 PP2;
	float2 ViewportSize; float2 PP3;
};

cbuffer HdrSettings
{
	float BloomStrength;
	float3 Paddingx;
};

#define kPointLightNum 4
#define kDirectionalLightNum 4
#define kSpotLightNum 4
#define kMaxShadowMapNum 4

cbuffer LightBuffer
{
	float4 AmbientLight;
};

cbuffer ShadowBuffer
{
	row_major matrix LightViewProjectionMatrix[kMaxShadowMapNum];
	row_major matrix LightViewMatrix[kMaxShadowMapNum];
	float4 LightTextureOffsets[kMaxShadowMapNum];
};

cbuffer MaterialBuffer
{
	float3 MaterialAlbedo; float MaterialRoughness;
	float MaterialRefractionIndex;
	float3 MaterialPadding;
};

SamplerState TextureSampler;
SamplerState SimpleSampler;
SamplerState LinearSampler;

float3 VSPositionFromDepth(float Depth, float2 vTexCoord)
{
	// Get the depth value for this pixel
	float z = Depth;

	// Get x/w and y/w from the viewport position
	float x = vTexCoord.x * 2.0 - 1.0;
	float y = (1 - vTexCoord.y) * 2.0 - 1.0;

	float4 ProjectedPosition = float4(x, y, z, 1.0f);

	// Transform by the inverse projection matrix
	float4 ViewSpacePosition = mul(ProjectedPosition, InverseProjectionMatrix);
	// Divide by w to get the view-space position
	return ViewSpacePosition.xyz / ViewSpacePosition.w;
}