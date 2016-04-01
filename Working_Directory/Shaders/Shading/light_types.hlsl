
void EvaluatePointLight(in float3 WorldPosition, in float4 LightGeometryData, in float4 LightParametersData, out float3 LightVector, out float3 LightColour, out float LightIntensity)
{
	float3 LightToPosition = LightGeometryData.xyz - WorldPosition;
	float LightDistance = length(LightToPosition);
	LightToPosition = normalize(LightToPosition);
	LightVector = normalize(LightToPosition);

	float Attenuation = max(1.0 - pow(saturate(LightDistance / (LightGeometryData.w)), 2), 0.0) * LightParametersData.w;
	LightColour = LightParametersData.xyz;
	LightIntensity = Attenuation;
}

void EvaluateSpotLight(in float3 WorldPosition, in float4 LightGeometryData, in float4 LightParameters1Data, in float4 LightParameters2Data, out float3 LightVector, out float3 LightColour, out float LightIntensity)
{
	float3 LightToPosition = LightGeometryData.xyz - WorldPosition;
	float LightDistance = length(LightToPosition);

	LightToPosition = normalize(LightToPosition);
	LightVector = normalize(-LightParameters2Data.xyz);

	float CurrentAngle = acos(dot(LightVector, LightToPosition));
	float SpotlightInclusion = CurrentAngle < LightParameters2Data.w ? 1 : 0;

	float Attenuation = 1.0 - pow(saturate(LightDistance / (LightGeometryData.w)), 2);

	LightIntensity = Attenuation * LightParameters1Data.w * SpotlightInclusion;
	LightColour = LightParameters1Data.xyz;
}