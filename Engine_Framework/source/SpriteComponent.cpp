#include "SpriteComponent.h"
#include "ClassType.h"
#include "EntitySet.h"
#include "Entity.h"
#include "DrawCallParameters.h"
#include "ConstantBufferTypes.h"
#include "GraphicsContext.h"
#include "MeshComponent.h"

FSpriteRendererSystem::FSpriteRendererSystem(FSpriteBatch* SpriteBatch, FGraphicsContext* GraphicsContext) : SpriteBatch(SpriteBatch), GraphicsContext(GraphicsContext)
{

}

void FSpriteRendererSystem::Render()
{
	for (auto Entity : Entities->Get())
	{
		auto SpriteComponent = Entity->GetComponent<FSpriteComponent>();
		auto TransformComponent = Entity->GetComponent<FTransformComponent>();

		SpriteComponent->Sprite.Position = glm::vec2(TransformComponent->Transform.Position);
		SpriteBatch->DrawSprite(SpriteComponent->Sprite);
	}

	FInstanceBuffer InstanceBuffer = FInstanceBuffer::Identity;
	FDrawCall DrawCall(GraphicsContext->GetImmediateContext(), &InstanceBuffer, nullptr);
	SpriteBatch->Render(DrawCall);
}

void FSpriteRendererSystem::OnAdded()
{
	Entities = GetEntitySet({ TYPE_OF(FSpriteComponent), TYPE_OF(FTransformComponent) });
}

void FSpriteRendererSystem::Update(float DeltaTime)
{
	for (auto Entity : Entities->Get())
	{
		auto SpriteComponent = Entity->GetComponent<FSpriteComponent>();
		SpriteComponent->Sprite.Rotation += DeltaTime * 50.0f;
	}
}
