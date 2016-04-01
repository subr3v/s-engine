#ifndef PointLightComponent_h__
#define PointLightComponent_h__

#include "BaseScene.h"
#include "Component.h"

class FPointLightComponent : public TComponent<FPointLightComponent>
{
public:
	FPointLightComponent()
	{
		memset(&Light, 0, sizeof(Light));
	}

	FPointLightComponent(const FPointLight& PointLight) : Light(PointLight) { }

	FPointLight Light;
};

class FDirectionalLightComponent : public TComponent<FDirectionalLightComponent>
{
public:
	FDirectionalLightComponent()
	{
		memset(&Light, 0, sizeof(Light));
	}

	FDirectionalLight Light;
};

class FSpotLightComponent : public TComponent<FSpotLightComponent>
{
public:
	FSpotLightComponent()
	{
		memset(&Light, 0, sizeof(Light));
	}

	FSpotLightComponent(const FSpotlight& SpotLight) : Light(SpotLight) { }

	FSpotlight Light;
};

#endif // PointLightComponent_h__
