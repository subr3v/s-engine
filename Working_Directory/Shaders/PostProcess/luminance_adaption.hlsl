Texture2D PreviousLuminanceTexture;
Texture2D CurrentLuminanceTexture;

#include "fullscreen_fast.hlsl"

#define fTau 1.55f

float Pixel(Vertex_Output Input) : SV_Target
{
	// Pattanaik's luminance adaption:
	float PreviousLuminance = PreviousLuminanceTexture.Load(uint3(0, 0, 0)).r;
	float CurrentLuminance = (CurrentLuminanceTexture.SampleLevel(SimpleSampler, float2(0.5, 0.5), 999.0f).r);

	float AdaptedLuminance = PreviousLuminance + (CurrentLuminance - PreviousLuminance) * (1 - exp(-FrameDelta * fTau));
	return AdaptedLuminance;
}
