#include "TextureConverter.h"
#include "TextureData.h"
#include "utImage.h"
#include <locale>

bool IsExtensionSupported(const std::string& Extension)
{
	return Extension == ".png" || Extension == ".jpg" || Extension == ".tga" || Extension == ".tif";
}

const char* GetTypeName()
{
	return "Texture";
}

bool ConvertAsset(const FAsset& Asset, FSerializer& Serializer)
{
	FTextureData TextureData;

	int Comp;
	int Width, Height;
	unsigned char* PixelData = Horde3D::stbi_load(Asset.GetFilename().c_str(), &Width, &Height, &Comp, 4);

	TextureData.Width = Width;
	TextureData.Height = Height;

	if (PixelData != nullptr)
	{
		TextureData.PixelData.Resize(TextureData.Width * TextureData.Height);
		memcpy(TextureData.PixelData.Data(), PixelData, TextureData.Width * TextureData.Height * sizeof(u32));
		Horde3D::stbi_image_free(PixelData);

		TextureData.Serialize(Serializer);

		return true;
	}

	return false;
}

