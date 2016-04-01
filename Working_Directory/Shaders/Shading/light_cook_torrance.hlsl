#define PI 3.14159265359f

float NormalDistribution_GGX(float Roughness, float NdH)
{
	// Isotropic ggx.
	float a2 = Roughness * Roughness;
	float NdH2 = NdH * NdH;

	float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
	denominator *= denominator;
	denominator *= PI;

	return a2 / denominator;
}

float NormalDistribution_Beckmann(float Roughness, float NdH)
{
	float a2 = Roughness * Roughness;
	float NdH2 = NdH * NdH;

	return (1.0f / (PI * a2 * NdH2 * NdH2 + 0.001)) * exp((NdH2 - 1.0f) / (a2 * NdH2));
}

float3 BRDF_Cook_Torrance(in float3 normal, in float3 viewer, in float3 light, in float3 Albedo, in float RoughnessValue, float RefractionIndex)
{
	float3 SpecularColour = Albedo * RoughnessValue;// float3(1.0, 1.0, 1.0);

	// Compute any aliases and intermediary values
	// -------------------------------------------
	float3 HalfVector = normalize((light + viewer));
	float NdotL = saturate(dot(normal, light));
	float NdotH = saturate(dot(normal, HalfVector));
	float NdotV = saturate(dot(normal, viewer));
	float VdotH = saturate(dot(viewer, HalfVector));

	// Evaluate the geometric term
	// --------------------------------
	float geo_numerator = 2.0f * NdotH;
	float geo_denominator = VdotH;

	float geo_b = (geo_numerator * NdotV) / geo_denominator;
	float geo_c = (geo_numerator * NdotL) / geo_denominator;
	float geo = min(1.0f, min(geo_b, geo_c));

	// Now evaluate the roughness term
	// -------------------------------
	float distribution = NormalDistribution_GGX(RoughnessValue, NdotH);


	// Next evaluate the Fresnel value
	// -------------------------------
	float fresnel = pow(1.0f - VdotH, 5.0f);
	fresnel *= (1.0f - RefractionIndex);
	fresnel += RefractionIndex;

	// Put all the terms together to compute
	// the specular term in the equation
	// -------------------------------------
	float3 Rs_numerator = (fresnel * geo * distribution);
	float Rs_denominator = max(NdotV * NdotL, 1e-06);
	float3 Rs = Rs_numerator / Rs_denominator;

	// Put all the parts together to generate
	// the final colour
	// --------------------------------------

	float3 cSpec = SpecularColour * Rs;
	float3 final = max(0.0f, NdotL) * (Albedo + cSpec);

	return final;

	// Return the result
	// -----------------
}