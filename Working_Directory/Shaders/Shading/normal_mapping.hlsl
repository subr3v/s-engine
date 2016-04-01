
Texture2D NormalTexture;

float3 DoNormalMapping(float2 Texcoord, float3 InputNormal, float3 InputTangent, float3 InputBiTangent)
{
#if defined(NORMAL_MAPPING)
	float3 Normal = (NormalTexture.Sample(TextureSampler, Texcoord).xyz * 2.0) - 1.0;

	//Create the "Texture Space"
	float3x3 TextureSpace = float3x3(InputTangent, InputBiTangent, InputNormal);

	//Convert normal from normal map to texture space and store in input.normal
	return normalize(mul(Normal, TextureSpace));
#endif

	return normalize(InputNormal);
}
