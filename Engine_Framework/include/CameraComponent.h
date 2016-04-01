#ifndef CameraComponent_h__
#define CameraComponent_h__

#include "Component.h"
#include "Camera.h"

class FCameraComponent : public TComponent<FCameraComponent>
{
public:
	FCameraComponent(const FCamera& Camera, bool bIsActive = true) : Camera(Camera), bIsActive(bIsActive) { }

	FCamera Camera;
	bool bIsActive;
};

#endif // CameraComponent_h__
