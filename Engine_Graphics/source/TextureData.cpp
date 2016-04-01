#include "TextureData.h"
#include "MemoryArchive.h"

void FTextureData::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeInt32(Width);
	Serializer.SerializeInt32(Height);
	Serializer.SerializeObjectArray(PixelData);
}
