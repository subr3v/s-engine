#ifndef SponzaScene_h__
#define SponzaScene_h__

#include "Scene.h"
#include "ResourceManager.h"
#include "CameraControllers.h"
#include "Camera.h"
#include "BaseScene.h"
#include "Entity.h"
#include "UniquePtr.h"
#include "SpriteBatch.h"
#include "ForwardPlusRenderer.h"
#include "World.h"

class FSponzaScene : public FScene
{
public:
	FSponzaScene();
	~FSponzaScene();

	virtual void Load() override;
	virtual void Unload() override;
	virtual void Render(float DeltaTime) override;
	virtual void Update(float DeltaTime) override;

private:
	TUniquePtr<FForwardPlusRenderer> SceneRenderer;
	TUniquePtr<FWorld> World;
	TUniquePtr<FResourceGroup> ResourceGroup;
	TUniquePtr<FSpriteBatch> SpriteBatch;
	TUniquePtr<FFreeFlyCameraController> CameraController;
};

#endif // SponzaScene_h__
