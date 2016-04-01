#ifndef Sprite_h__
#define Sprite_h__

#include <glm/glm.hpp>
#include <d3d11.h>

class FTexture;

class FSprite
{
public:
	FSprite() { }
	FSprite(FTexture* Texture, float X, float Y) : Position(X, Y) { SetFullTexture(Texture); }
	FSprite(ID3D11ShaderResourceView* TextureResource, float X, float Y, float Width, float Height);

	glm::vec2 Size = glm::vec2(1.0f, 1.0f);
	glm::vec2 Origin = glm::vec2(0.0f, 0.0f);

	glm::vec2 Position = glm::vec2(0.0f, 0.0f);
	glm::vec2 Scale = glm::vec2(1.0f, 1.0f);
	float Rotation = 0.0f;

	glm::vec4 Colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	ID3D11ShaderResourceView* ColourTexture = nullptr;
	glm::vec2 UvCoordinate = glm::vec2(0.0f, 0.0f);
	glm::vec2 UvSize = glm::vec2(1.0f, 1.0f);
	
	void SetUvCoordinateFromTexture(FTexture* ColourTexture, int X, float Y, int Width, int Height);
	void SetFullTexture(FTexture* ColourTexture);
private:
	
};

#endif // Sprite_h__
