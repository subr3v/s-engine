#include "buffers.hlsl"
#include "tiled_lighting.hlsl"

#define FLT_MAX         3.402823466e+38F
#define LIGHTS_PER_THREAD (TILE_SIZE * TILE_SIZE)

Buffer<float4> PointLightGeometryData : register(t0);
Buffer<float4> SpotLightGeometryData : register(t1);
Texture2D<float> DepthTexture : register(t2);

RWBuffer<uint> TileToIndexMap : register(u0);

// Shared memory for one tile
groupshared uint TilePointLightCounter;
groupshared uint TileSpotLightCounter;
groupshared uint TilePointLightsIdList[MAX_POINT_LIGHTS_PER_TILE];
groupshared uint TileSpotLightsIdList[MAX_SPOT_LIGHTS_PER_TILE];
groupshared uint MinimumZValue;
groupshared uint MaximumZValue;

// Implementation to compute frustrum culling and equation from AMD DX 11 Sample
// this creates the standard Hessian-normal-form plane equation from three points, 
// except it is simplified for the case where the first point is the origin
float3 CreatePlaneEquation(float3 b, float3 c)
{
	// normalize(cross( b-a, c-a )), except we know "a" is the origin
	// also, typically there would be a fourth term of the plane equation, 
	// -(n dot a), except we know "a" is the origin
	return normalize(cross(b, c));;
}

// point-plane distance, simplified for the case where 
// the plane passes through the origin
float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
	// dot(eqn.xyz,p) + eqn.w, , except we know eqn.w is zero 
	// (see CreatePlaneEquation above)
	return dot(eqn, p);
}

// convert a point from post-projection space into view space
float4 ConvertProjToView(float4 p)
{
	p = mul(p, InverseProjectionMatrix);
	p /= p.w;
	return p;
}

bool TestFrustumSides(float3 c, float r, float3 plane0, float3 plane1, float3 plane2, float3 plane3)
{
	bool intersectingOrInside0 = GetSignedDistanceFromPlane(c, plane0) < r;
	bool intersectingOrInside1 = GetSignedDistanceFromPlane(c, plane1) < r;
	bool intersectingOrInside2 = GetSignedDistanceFromPlane(c, plane2) < r;
	bool intersectingOrInside3 = GetSignedDistanceFromPlane(c, plane3) < r;

	return (intersectingOrInside0 && intersectingOrInside1 &&
		intersectingOrInside2 && intersectingOrInside3);
}

// convert a depth value from post-projection space into view space
float ConvertProjDepthToView(float z)
{
	z = 1.f / (z * InverseProjectionMatrix._34 + InverseProjectionMatrix._44);
	return z;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void mainCS(uint3 GlobalIDs : SV_DispatchThreadID, uint3 LocalIDs : SV_GroupThreadID, uint3 GroupIDs : SV_GroupID)
{
	uint LocalThreadIndex = LocalIDs.x + LocalIDs.y * TILE_SIZE;

	if (LocalThreadIndex == 0)
	{
		TilePointLightCounter = 0;
		TileSpotLightCounter = 0;
		MinimumZValue = 0x7f7fffff;
		MaximumZValue = 0;
	}

	float3 frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3;
	{   // construct frustum for this tile
		uint pxm = TILE_SIZE * GroupIDs.x;
		uint pym = TILE_SIZE * GroupIDs.y;
		uint pxp = TILE_SIZE * (GroupIDs.x + 1);
		uint pyp = TILE_SIZE * (GroupIDs.y + 1);

		uint uWindowWidthEvenlyDivisibleByTileRes = TILE_SIZE * GetNumTilesX();
		uint uWindowHeightEvenlyDivisibleByTileRes = TILE_SIZE * GetNumTilesY();

		// four corners of the tile, clockwise from top-left
		float3 frustum0 = ConvertProjToView(float4(pxm / (float)uWindowWidthEvenlyDivisibleByTileRes*2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pym) / (float)uWindowHeightEvenlyDivisibleByTileRes*2.f - 1.f, 1.f, 1.f)).xyz;
		float3 frustum1 = ConvertProjToView(float4(pxp / (float)uWindowWidthEvenlyDivisibleByTileRes*2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pym) / (float)uWindowHeightEvenlyDivisibleByTileRes*2.f - 1.f, 1.f, 1.f)).xyz;
		float3 frustum2 = ConvertProjToView(float4(pxp / (float)uWindowWidthEvenlyDivisibleByTileRes*2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pyp) / (float)uWindowHeightEvenlyDivisibleByTileRes*2.f - 1.f, 1.f, 1.f)).xyz;
		float3 frustum3 = ConvertProjToView(float4(pxm / (float)uWindowWidthEvenlyDivisibleByTileRes*2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pyp) / (float)uWindowHeightEvenlyDivisibleByTileRes*2.f - 1.f, 1.f, 1.f)).xyz;

		// create plane equations for the four sides of the frustum, 
		// with the positive half-space outside the frustum (and remember, 
		// view space is left handed, so use the left-hand rule to determine 
		// cross product direction)
		frustumEqn0 = CreatePlaneEquation(frustum0, frustum1);
		frustumEqn1 = CreatePlaneEquation(frustum1, frustum2);
		frustumEqn2 = CreatePlaneEquation(frustum2, frustum3);
		frustumEqn3 = CreatePlaneEquation(frustum3, frustum0);
	}

	GroupMemoryBarrierWithGroupSync();

	// Calculate min and max depth for culling.

	float depth = DepthTexture.Load(uint3(GlobalIDs.x, GlobalIDs.y, 0)).x;
	float viewPosZ = ConvertProjDepthToView(depth);
	uint z = asuint(viewPosZ);
	if (depth != 0.f)
	{
		InterlockedMax(MaximumZValue, z);
		InterlockedMin(MinimumZValue, z);
	}

	GroupMemoryBarrierWithGroupSync();

	float maxZ = asfloat(MaximumZValue);
	float minZ = asfloat(MinimumZValue);

	uint PointLightCount = NumLights & 0xFFFFu;
	for (uint i = LocalThreadIndex; i < PointLightCount; i += LIGHTS_PER_THREAD)
	{
		float4 center = PointLightGeometryData[i];
		float r = center.w * 1.25;
		center.xyz = mul(float4(center.xyz, 1), ViewMatrix).xyz;

		// Project light and frustrum cull
		// test if sphere is intersecting or inside frustum
		if (TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3))
		{
			if (center.z + minZ < r && -center.z - maxZ < r)
			{
				uint DestinationId = 0;
				InterlockedAdd(TilePointLightCounter, 1, DestinationId);
				if (DestinationId > MAX_POINT_LIGHTS_PER_TILE - 1)
					break;
				TilePointLightsIdList[DestinationId] = i;
			}
		}
	}

	uint SpotLightCount = (NumLights & 0xFFFF0000u) >> 16;
	for (uint j = LocalThreadIndex; j < SpotLightCount; j += LIGHTS_PER_THREAD)
	{
		float4 center = SpotLightGeometryData[j];
		float r = center.w * 1.25;
		center.xyz = mul(float4(center.xyz, 1), ViewMatrix).xyz;

		// Project light and frustrum cull
		// test if sphere is intersecting or inside frustum
		if (TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3))
		{
			if (center.z + minZ < r && -center.z - maxZ < r)
			{
				uint DestinationId = 0;
				InterlockedAdd(TileSpotLightCounter, 1, DestinationId);
				if (DestinationId > MAX_SPOT_LIGHTS_PER_TILE - 1)
					break;
				TileSpotLightsIdList[DestinationId] = j;
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	{
		uint TileId = GroupIDs.x + GroupIDs.y * GetNumTilesX();
		uint StartOffset = MAX_LIGHTS_PER_TILE * TileId;

		for (uint k = LocalThreadIndex; k < TilePointLightCounter; k += LIGHTS_PER_THREAD)
		{
			TileToIndexMap[StartOffset + k] = TilePointLightsIdList[k];
		}

		for (uint h = LocalThreadIndex; h < TileSpotLightCounter; h += LIGHTS_PER_THREAD)
		{
			TileToIndexMap[StartOffset + (TilePointLightCounter + 1) + h] = TileSpotLightsIdList[h];
		}

		if (LocalThreadIndex == 0)
		{
			TileToIndexMap[StartOffset + TilePointLightCounter] = END_OF_LIST_SENTINEL;
			TileToIndexMap[StartOffset + (TilePointLightCounter + 1) + TileSpotLightCounter] = END_OF_LIST_SENTINEL;
		}
	}
}
