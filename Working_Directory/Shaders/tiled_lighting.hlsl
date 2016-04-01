#define MAX_POINT_LIGHTS_PER_TILE 64
#define MAX_SPOT_LIGHTS_PER_TILE 64
#define MAX_LIGHTS_PER_TILE (MAX_POINT_LIGHTS_PER_TILE + MAX_SPOT_LIGHTS_PER_TILE)
#define END_OF_LIST_SENTINEL 0xFFFF
#define TILE_SIZE 16

uint GetNumTilesX()
{
	return (uint)((ViewportSize.x + TILE_SIZE - 1) / (float)TILE_SIZE);
}

// calculate the number of tiles in the vertical direction
uint GetNumTilesY()
{
	return (uint)((ViewportSize.y + TILE_SIZE - 1) / (float)TILE_SIZE);
}

uint GetTileIndex(float4 ScreenPosition)
{
	return floor(ScreenPosition.x / TILE_SIZE) + floor(ScreenPosition.y / TILE_SIZE) * GetNumTilesX();
}

uint GetTileIndexStart(float4 ScreenPosition)
{
	return GetTileIndex(ScreenPosition) * MAX_LIGHTS_PER_TILE;
}