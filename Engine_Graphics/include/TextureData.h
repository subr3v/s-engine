#ifndef TextureData_h__
#define TextureData_h__

#include <cstdint>
#include <vector>
#include "Serializer.h"

// Texture data saved in 32 bpp RGBA format.
struct FTextureData
{
	s32 Width;
	s32 Height;
	TVector<u32> PixelData;

	void Serialize(FSerializer& Serializer);
};

#endif // TextureData_h__
