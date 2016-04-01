#include "buffers.hlsl"
#include "tiled_lighting.hlsl"

Texture2D DiffuseTexture;
Texture2D RoughnessTexture;
Texture2D RefractionTexture;

#include "Shading/normal_mapping.hlsl"
#include "Shading/shadow_mapping.hlsl"
#include "Shading/occlusion_mapping.hlsl"
#include "Shading/light_cook_torrance.hlsl"
#include "Shading/light_types.hlsl"

Buffer<float4> PointLightGeometryData;
Buffer<float4> PointLightParameterData;
Buffer<float4> SpotLightGeometryData;
Buffer<float4> SpotLightParameterData;
Buffer<uint> TileToIndexMap;

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
	float3 WorldSpacePosition : TEXCOORD2;
};

// Vertex shader code
Vertex_Output Vertex(Vertex_Input Input)
{
	Vertex_Output Output;

	Output.Position = mul(float4(Input.Position, 1.0), WorldMatrix);
	Output.WorldSpacePosition = Output.Position.xyz;
	Output.Position = mul(Output.Position, ViewMatrix);
	Output.Position = mul(Output.Position, ProjectionMatrix);
	Output.Texcoord = Input.Texcoord;
	Output.Normal = normalize(mul(float4(Input.Normal, 1.0), InverseTransposeWorldMatrix)).xyz;
	Output.Tangent = normalize(mul(float4(Input.Tangent, 1.0), InverseTransposeWorldMatrix)).xyz;
	Output.BiTangent = normalize(mul(float4(Input.BiTangent, 1.0), InverseTransposeWorldMatrix)).xyz;

	return Output;
}

float4 Pixel(Vertex_Output Input) : SV_TARGET
{
	float3 WorldPosition = Input.WorldSpacePosition;
	float3 ViewDirection = normalize(CameraPosition - WorldPosition);

	float3 Normal = DoNormalMapping(Input.Texcoord, Input.Normal, Input.Tangent, Input.BiTangent);
	Normal = normalize(Normal);

	float3 Diffuse; float Roughness; float RefractionIndex;

#if defined(USE_ALBEDO_TEXTURE)
	Diffuse = DiffuseTexture.Sample(TextureSampler, Input.Texcoord).rgb;
#else
	Diffuse = MaterialAlbedo;
#endif

#if defined(USE_ROUGHNESS_TEXTURE)
	Roughness = RoughnessTexture.Sample(TextureSampler, Input.Texcoord).r;
#else
	Roughness = MaterialRoughness;
#endif

#if defined(USE_REFRACTION_TEXTURE)
	RefractionIndex = RefractionTexture.Sample(TextureSampler, Input.Texcoord).r;
#else
	RefractionIndex = MaterialRefractionIndex;
#endif

	float Ambient = EvaluateAmbientOcclusion(Input.Position);
	float3 FinalColour = Ambient * Diffuse * AmbientLight.xyz;

	float LightCount = 0;
	uint NextLightIndex = GetTileIndexStart(Input.Position);
	uint LightIndex = TileToIndexMap[NextLightIndex];

	// Point lights first.
	[loop]
	while (LightIndex != END_OF_LIST_SENTINEL)
	{
		float3 LightVector;
		float3 LightColour;
		float LightIntensity;
		EvaluatePointLight(WorldPosition, PointLightGeometryData[LightIndex], PointLightParameterData[LightIndex], LightVector, LightColour, LightIntensity);

		if (LightIntensity > 0)
		{
			float3 BRDF_Term = BRDF_Cook_Torrance(Normal, ViewDirection, LightVector, Diffuse * LightColour, Roughness, RefractionIndex);
			FinalColour += BRDF_Term * LightIntensity;
			LightCount++;
		}

		NextLightIndex++;
		LightIndex = TileToIndexMap[NextLightIndex];
	}

	NextLightIndex++;
	LightIndex = TileToIndexMap[NextLightIndex];

	// Spotlights now.
	[loop]
	while (LightIndex != END_OF_LIST_SENTINEL)
	{
		float3 LightVector;
		float3 LightColour;
		float LightIntensity;
		EvaluateSpotLight(WorldPosition, SpotLightGeometryData[LightIndex], SpotLightParameterData[LightIndex * 3], SpotLightParameterData[LightIndex * 3 + 1], LightVector, LightColour, LightIntensity);

		float ShadowFactor = EvaluateShadow(SpotLightParameterData[LightIndex * 3 + 2].x, Input.WorldSpacePosition, SpotLightParameterData[LightIndex * 3 + 2].y);
		if (LightIntensity > 0)
		{
			float3 BRDF_Term = BRDF_Cook_Torrance(Normal, ViewDirection, LightVector, Diffuse * LightColour, Roughness, RefractionIndex);
			FinalColour += BRDF_Term * LightIntensity * ShadowFactor;
			LightCount++;
		}

		NextLightIndex++;
		LightIndex = TileToIndexMap[NextLightIndex];
	}

	// TODO: Alpha masking/ translucency, opacity mapping here.
	return float4(FinalColour, 1.0);
}
