#ifndef SpriteComponent_h__
#define SpriteComponent_h__

#include "Sprite.h"
#include "Transform.h"
#include "System.h"
#include "Component.h"
#include "SpriteBatch.h"

class FSpriteComponent : public TComponent<FSpriteComponent>
{
public:
	FSprite Sprite;
	bool bFollowRotation = true;
};

class FSpriteRendererSystem : public TGameSystem<FSpriteRendererSystem>
{
public:
	FSpriteRendererSystem(FSpriteBatch* SpriteBatch, FGraphicsContext* GraphicsContext);

private:
	virtual void Render() override;
	virtual void OnAdded() override;

	virtual void Update(float DeltaTime) override;

	FEntitySet* Entities;
	FSpriteBatch* SpriteBatch;
	FGraphicsContext* GraphicsContext;
};

#endif // SpriteComponent_h__
