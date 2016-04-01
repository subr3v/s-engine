#ifndef CameraSystem_h__
#define CameraSystem_h__
#include "System.h"
#include "CameraControllers.h"

class FCameraSystem : public TGameSystem<FCameraSystem>
{
public:

	virtual void OnAdded() override;
	virtual void Update(float DeltaTime) override;

	void SetCameraController(ICameraController* CameraController);

	void SetActive(bool bActive);

private:
	FEntitySet* CameraEntities;
	ICameraController* CameraController = nullptr;
	bool bIsActive = true;
};

#endif // CameraSystem_h__
