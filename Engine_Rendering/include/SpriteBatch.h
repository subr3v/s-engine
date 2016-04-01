#ifndef SpriteBatch_h__
#define SpriteBatch_h__

#include "DrawCallParameters.h"
#include "Sprite.h"
#include "Vector.h"
#include "UniquePtr.h"
#include "GraphicsBuffer.h"

class FGraphicsBuffer;
class FMaterial;
class FGraphicsContext;

struct FSpriteVertex
{
	glm::vec2 Position;
	glm::vec2 Size;
	float Rotation;
	glm::vec2 Origin;
	glm::vec2 UvCoordinate;
	glm::vec2 UvSize;
	unsigned char Colour[4];
};

namespace std
{
	template <>
	struct alignment_of<FSpriteVertex>
	{
		enum
		{
			value = 16
		};
	};
}

class FSpriteBatch
{
public:
	FSpriteBatch(FGraphicsContext* GraphicsContext, FMaterial* Material, int MaxSprites = 0);
	~FSpriteBatch();

	void Render(const FDrawCall& DrawCall);

	void SetDefaultMaterial(FMaterial* Material);
	void DrawSprite(const FSprite& Sprite);

private:
	void ResizeBuffers(int NewSize);
	void UploadVertexData(ID3D11DeviceContext* DeviceContext);

	FMaterial* DefaultMaterial;
	FGraphicsContext* GraphicsContext;

	TVector<FSprite> Sprites;
	SizeType MaxSprites = 0;
	TUniquePtr<FGraphicsBuffer> VertexBuffer;

	typedef unsigned int IndexType;
};

#endif // SpriteBatch_h__
