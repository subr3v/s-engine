#include "Sprite.h"
#include "Texture.h"


void FSprite::SetUvCoordinateFromTexture(FTexture* ColourTexture, int X, float Y, int Width, int Height)
{
	this->ColourTexture = ColourTexture->GetResourceView();

	if (ColourTexture == nullptr)
	{
		//TODO: assert invalid operation.
		return;
	}

	Size.x = (float)Width;
	Size.y = (float)Height;

	UvSize.x = (float)Width / ColourTexture->GetWidth();
	UvSize.y = (float)Width / ColourTexture->GetHeight();

	UvCoordinate.x = (float)X / ColourTexture->GetWidth();
	UvCoordinate.y = (float)Y / ColourTexture->GetHeight();
}

void FSprite::SetFullTexture(FTexture* ColourTexture)
{
	this->ColourTexture = ColourTexture->GetResourceView();

	if (ColourTexture == nullptr)
	{
		//TODO: assert invalid operation.
		return;
	}

	Size.x = (float)ColourTexture->GetWidth();
	Size.y = (float)ColourTexture->GetHeight();

	UvSize.x = 1.0f;
	UvSize.y = 1.0f;

	UvCoordinate.x = 0.0f;
	UvCoordinate.y = 0.0f;
}

FSprite::FSprite(ID3D11ShaderResourceView* TextureResource, float X, float Y, float Width, float Height) : Position(X, Y), Size(Width, Height)
{
	this->ColourTexture = TextureResource;

	UvSize.x = 1.0f;
	UvSize.y = 1.0f;

	UvCoordinate.x = 0.0f;
	UvCoordinate.y = 0.0f;
}
