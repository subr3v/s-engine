#ifndef CameraControllers_h__
#define CameraControllers_h__
#include "Spline.h"

class ICameraController
{
public:
	ICameraController(class FWindow* Window, class FInput* InputManager) : Window(Window), InputManager(InputManager) { }
	virtual ~ICameraController() { }

	virtual void Reset(class FCamera* Camera) { };
	virtual void Update(float DeltaTime, class FCamera* Camera) { };

protected:
	class FWindow* Window;
	class FInput* InputManager;
};

class FBaseInputCameraController : public ICameraController
{
public:
	FBaseInputCameraController(class FWindow* Window, class FInput* InputManager);
	virtual void Update(float DeltaTime, class FCamera* Camera) override;
	bool bMouseYaw;
	bool bMousePitch;
	float CameraSpeed;
	float CameraRotationSpeed;
	bool bMoveKeyboard;
};

class FXYPlaneCameraController : public FBaseInputCameraController
{
public:
	FXYPlaneCameraController(class FWindow* Window, class FInput* InputManager);
	virtual void Reset(class FCamera* Camera) override;
	virtual void Update(float DeltaTime, class FCamera* Camera) override;
};

class FFreeFlyCameraController : public FBaseInputCameraController
{
public:
	FFreeFlyCameraController(class FWindow* Window, class FInput* InputManager);
	virtual void Reset(class FCamera* Camera) override;
};

class FLookCameraController : public FBaseInputCameraController
{
public:
	FLookCameraController(class FWindow* Window, class FInput* InputManager);
};

class FSplineCameraController : public FBaseInputCameraController
{
public:
	FSplineCameraController(const FSpline& Spline, float DurationInSeconds, class FWindow* Window, class FInput* InputManager);

	virtual void Reset(class FCamera* Camera) override;
	virtual void Update(float DeltaTime, class FCamera* Camera) override;

private:
	FSpline SplinePath;

	float CurrentTime;
	float Duration;
};

#endif // CameraControllers_h__
