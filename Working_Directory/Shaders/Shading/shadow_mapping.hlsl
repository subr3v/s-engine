Texture2D ShadowTexture;

float EvaluateShadow(in float Enabled, in float3 WorldSpacePosition, int ShadowMapIndex)
{
#if defined(SHADOW_MAPPING)
	if (Enabled > 0)
	{
		float4 ShadowPosition = mul(float4(WorldSpacePosition, 1.0), LightViewProjectionMatrix[ShadowMapIndex]);
		float2 ShadowTexCoordinate = (ShadowPosition.xy / ShadowPosition.w) * 0.5 + float2(0.5, 0.5);
		ShadowTexCoordinate.y = 1 - ShadowTexCoordinate.y;
		ShadowTexCoordinate = ShadowTexCoordinate * LightTextureOffsets[ShadowMapIndex].xy + LightTextureOffsets[ShadowMapIndex].zw;

		float Visibility = ShadowTexture.Sample(TextureSampler, ShadowTexCoordinate).r;
		float TestValue = (ShadowPosition.z / ShadowPosition.w) - 0.000005;
		
		if (TestValue > Visibility)
		{
			return 0.0;
		}

		return 1.0;
	}
#endif

	// No shadows
	return 1.0;
}