#ifndef F3DRendererSystem_h__
#define F3DRendererSystem_h__

#include <vector>
#include "System.h"
#include "BaseScene.h"
#include "DrawCallParameters.h"
#include "BaseScene.h"

class FForwardPlusRenderer;

class FRendererSystem : public TGameSystem<FRendererSystem>, public IScene
{
public:
	FRendererSystem(class FForwardPlusRenderer* Renderer);
	~FRendererSystem();

	void SetAmbientLight(const FAmbientLight& AmbientLight);

	FForwardPlusRenderer* GetRenderer();
private:
	virtual void Draw(FDrawCall Params) override;

	virtual const TVector<FPointLight>& GetPointLights() const override { return PointLights; }
	virtual const TVector<FDirectionalLight>& GetDirectionalLights() const override { return DirectionalLights; }
	virtual const TVector<FSpotlight>& GetSpotLights() const override { return SpotLights; }
	virtual const FAmbientLight& GetAmbientLight() const override { return AmbientLight; }

	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void Update(float DeltaTime) override;

	virtual void OnAdded() override;

	TVector<FSpotlight> SpotLights;
	TVector<FDirectionalLight> DirectionalLights;
	TVector<FPointLight> PointLights;
	FAmbientLight AmbientLight;

	FForwardPlusRenderer* Renderer;

	// Light related entities set.
	FEntitySet* PointLightEntities;
	FEntitySet* SpotLightEntities;
	FEntitySet* DirectionalLightEntities;

	// Camera related entities set
	FEntitySet* CameraEntities;

	// Geometry related entities set
	FEntitySet* ModelEntities;
};

#endif // F3DRendererSystem_h__
