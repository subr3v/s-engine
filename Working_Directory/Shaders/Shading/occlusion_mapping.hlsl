Texture2D OcclusionTexture;

float EvaluateAmbientOcclusion(float4 ScreenPosition)
{
#if SSAO_ENABLED
	float AmbientOcclusion = OcclusionTexture.Sample(SimpleSampler, ScreenPosition.xy / ViewportSize).r;
#else
	float AmbientOcclusion = 1;
#endif

	return clamp(AmbientOcclusion, 0.0, 1.0);
}