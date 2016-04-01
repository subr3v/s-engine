#include "SpriteBatch.h"
#include "GraphicsBuffer.h"
#include "Material.h"
#include "GraphicsContext.h"
#include <glm/gtx/rotate_vector.hpp>
#include "ConstantBufferTypes.h"
#include "Texture.h"
#include "RenderState.h"
#include "MemoryUtils.h"

FSpriteBatch::FSpriteBatch(FGraphicsContext* GraphicsContext, FMaterial* Material, int MaxSprites) : DefaultMaterial(Material), MaxSprites(MaxSprites), GraphicsContext(GraphicsContext)
{
	ResizeBuffers(MaxSprites);
}

FSpriteBatch::~FSpriteBatch()
{

}

void FSpriteBatch::Render(const FDrawCall& DrawCall)
{
	if (Sprites.Size() >= MaxSprites)
	{
		ResizeBuffers(Sprites.Size() * 2);
	}

	UploadVertexData(DrawCall.DeviceContext);

	const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	DrawCall.DeviceContext->OMSetBlendState(GraphicsContext->GetRenderStates()->AlphaBlendState, BlendFactor, 0xFFFFFFF);
	DrawCall.DeviceContext->RSSetState(GraphicsContext->GetRenderStates()->SolidNoCullRasterizer);

	ID3D11Buffer* Vb = VertexBuffer->GetBuffer();
	UINT Offsets = 0;
	UINT VertexSize = sizeof(FSpriteVertex);
	DrawCall.DeviceContext->IASetVertexBuffers(0, 1, &Vb, &VertexSize, &Offsets);
	DrawCall.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	FMaterialParameters* Parameters = DefaultMaterial->GetParameters();
	if (DrawCall.InstanceData != nullptr)
	{
		Parameters->UpdateUniform(DrawCall.DeviceContext, FInstanceBuffer::kBufferName, DrawCall.InstanceData, sizeof(FInstanceBuffer));
	}

	DefaultMaterial->Apply(DrawCall.DeviceContext);

	ID3D11ShaderResourceView* CurrentTexture = nullptr;

	int StartVertex = 0;
	int SpriteCount = 0;

	for (SizeType i = 0; i < Sprites.Size(); ++i)
	{
		const FSprite& Sprite = Sprites[i];
		if (CurrentTexture != Sprite.ColourTexture)
		{
			// Flush
			if (SpriteCount > 0)
			{
				DrawCall.Draw(SpriteCount, StartVertex);
				StartVertex += SpriteCount;
			}

			SpriteCount = 1;
			CurrentTexture = Sprite.ColourTexture;
			Parameters->SetResource(FTextureNames::kDiffuseMap, CurrentTexture);
			Parameters->Apply(DrawCall.DeviceContext);
		}
		else
		{
			++SpriteCount;
		}
	}

	if (SpriteCount > 0)
	{
		DrawCall.Draw(SpriteCount, StartVertex);
	}

	DrawCall.DeviceContext->RSSetState(GraphicsContext->GetRenderStates()->SolidRasterizer);
	DrawCall.DeviceContext->OMSetBlendState(GraphicsContext->GetRenderStates()->OpaqueBlendState, BlendFactor, 0xFFFFFFF);
	Sprites.Clear();
}

void FSpriteBatch::DrawSprite(const FSprite& Sprite)
{
	Sprites.Add(Sprite);
}

void FSpriteBatch::SetDefaultMaterial(FMaterial* Material)
{
	this->DefaultMaterial = Material;
}

void FSpriteBatch::ResizeBuffers(int NewSize)
{
	this->MaxSprites = NewSize;

	if (MaxSprites > 0)
	{
		VertexBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, EBufferType::Vertex, EBufferUsage::Dynamic, sizeof(FSpriteVertex) * MaxSprites, nullptr);
	}
}

void FSpriteBatch::UploadVertexData(ID3D11DeviceContext* DeviceContext)
{
	int VertexIndex = 0;

	FSpriteVertex* Vertices = (FSpriteVertex*)VertexBuffer->MapData(DeviceContext);

	for (SizeType i = 0; i < Sprites.Size(); ++i)
	{
		const FSprite& Sprite = Sprites[i];

		FSpriteVertex& SpriteVertex = Vertices[VertexIndex++];

		SpriteVertex.Colour[0] = (u8)(Sprite.Colour.r * 255.0f);
		SpriteVertex.Colour[1] = (u8)(Sprite.Colour.g * 255.0f);
		SpriteVertex.Colour[2] = (u8)(Sprite.Colour.b * 255.0f);
		SpriteVertex.Colour[3] = (u8)(Sprite.Colour.a * 255.0f);

		SpriteVertex.Origin = Sprite.Origin * Sprite.Scale;
		SpriteVertex.Size = Sprite.Size * Sprite.Scale;
		SpriteVertex.Rotation = glm::radians(Sprite.Rotation);
		SpriteVertex.UvCoordinate = Sprite.UvCoordinate;
		SpriteVertex.UvSize = Sprite.UvSize;
		SpriteVertex.Position = Sprite.Position;

		/*
		glm::vec2 TopLeft = -ScaledOrigin;

		if (Sprite.Rotation != 0)
		{
			v0.Position = Sprite.Position + glm::rotate(TopLeft, RadiansRotation);
			v1.Position = Sprite.Position + glm::rotate(TopLeft + glm::vec2(SpriteSize.x, 0.0f) * 0.5f, RadiansRotation);
			v2.Position = Sprite.Position + glm::rotate(TopLeft + glm::vec2(SpriteSize.x, SpriteSize.y) * 0.5f, RadiansRotation);
			v3.Position = Sprite.Position + glm::rotate(TopLeft + glm::vec2(0.0f, SpriteSize.y) * 0.5f, RadiansRotation);
		}
		else
		{
			v0.Position = Sprite.Position + TopLeft;
			v1.Position = Sprite.Position + TopLeft + glm::vec2(SpriteSize.x, 0.0f);
			v2.Position = Sprite.Position + TopLeft + glm::vec2(SpriteSize.x, SpriteSize.y);
			v3.Position = Sprite.Position + TopLeft + glm::vec2(0.0f, SpriteSize.y);
		}

		v0.Texcoord = Sprite.UvCoordinate;
		v1.Texcoord = Sprite.UvCoordinate + glm::vec2(Sprite.UvSize.x, 0.0f);
		v2.Texcoord = Sprite.UvCoordinate + glm::vec2(Sprite.UvSize.x, Sprite.UvSize.y);
		v3.Texcoord = Sprite.UvCoordinate + glm::vec2(0.0f, Sprite.UvSize.y);

		if (false) // Flip Horizontal
		{
			std::swap(v0.Texcoord, v1.Texcoord);
			std::swap(v2.Texcoord, v3.Texcoord);
		}

		if (false) // Flip Vertical/
		{
			std::swap(v0.Texcoord, v3.Texcoord);
			std::swap(v1.Texcoord, v2.Texcoord);
		}
		*/
	}

	VertexBuffer->UnMapData(DeviceContext);
}

