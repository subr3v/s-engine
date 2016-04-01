#include "CameraControllers.h"
#include "Input.h"
#include "Camera.h"

FBaseInputCameraController::FBaseInputCameraController(class FWindow* Window, class FInput* InputManager) : ICameraController(Window, InputManager)
, bMouseYaw(true), bMousePitch(true), CameraSpeed(300.0f), CameraRotationSpeed(10.0f), bMoveKeyboard(true)
{

}

void FBaseInputCameraController::Update(float DeltaTime, class FCamera* Camera)
{
	if (bMoveKeyboard)
	{
		if (InputManager->IsKeyDown('W'))
		{
			Camera->SetPosition(Camera->GetPosition() + Camera->GetForwardVector() * CameraSpeed * DeltaTime);
		}
		if (InputManager->IsKeyDown('S'))
		{
			Camera->SetPosition(Camera->GetPosition() - Camera->GetForwardVector() * CameraSpeed * DeltaTime);
		}
		if (InputManager->IsKeyDown('D'))
		{
			Camera->SetPosition(Camera->GetPosition() + Camera->GetRightVector() * CameraSpeed * DeltaTime);
		}
		if (InputManager->IsKeyDown('A'))
		{
			Camera->SetPosition(Camera->GetPosition() - Camera->GetRightVector() * CameraSpeed * DeltaTime);
		}

		if (InputManager->IsKeyDown('E'))
		{
			Camera->SetYaw(Camera->GetYaw() + (DeltaTime * CameraRotationSpeed));
		}
		if (InputManager->IsKeyDown('Q'))
		{
			Camera->SetYaw(Camera->GetYaw() - (DeltaTime * CameraRotationSpeed));
		}
	}

	glm::vec2 CurrentMousePosition = InputManager->GetMousePosition();

	float YawDifference = (Window->GetWidth() * 0.5f) - CurrentMousePosition.x;
	float PitchDifference = (Window->GetHeight() * 0.5f) - CurrentMousePosition.y;

	if (bMouseYaw)
		Camera->SetYaw(Camera->GetYaw() - (YawDifference * DeltaTime * CameraRotationSpeed));

	if (bMousePitch)
		Camera->SetPitch(Camera->GetPitch() + (PitchDifference * DeltaTime * CameraRotationSpeed));

	if (bMouseYaw || bMousePitch)
		InputManager->CenterMouseScreen(Window);
}

FXYPlaneCameraController::FXYPlaneCameraController(class FWindow* Window, class FInput* InputManager) : FBaseInputCameraController(Window, InputManager)
{
	bMousePitch = false;
	bMouseYaw = false;
}

void FXYPlaneCameraController::Reset(class FCamera* Camera)
{
	Camera->SetPosition(glm::vec3(0.0f, 25.0f, 50.0f));
	Camera->SetYaw(0.0f);
	Camera->SetPitch(0.0f);
}

void FXYPlaneCameraController::Update(float DeltaTime, class FCamera* Camera)
{
	Camera->SetPitch(0.0f);
	FBaseInputCameraController::Update(DeltaTime, Camera);
}

FFreeFlyCameraController::FFreeFlyCameraController(class FWindow* Window, class FInput* InputManager) : FBaseInputCameraController(Window, InputManager)
{

}

void FFreeFlyCameraController::Reset(class FCamera* Camera)
{
}

FLookCameraController::FLookCameraController(class FWindow* Window, class FInput* InputManager) : FBaseInputCameraController(Window, InputManager)
{
	bMoveKeyboard = false;
}

FSplineCameraController::FSplineCameraController(const FSpline& Spline, float DurationInSeconds, class FWindow* Window, class FInput* InputManager) 
	: FBaseInputCameraController(Window, InputManager), SplinePath(Spline), CurrentTime(0.0f), Duration(DurationInSeconds)
{
	bMoveKeyboard = false;
}

void FSplineCameraController::Reset(class FCamera* Camera)
{
	CurrentTime = 0;
}

void FSplineCameraController::Update(float DeltaTime, class FCamera* Camera)
{
	FBaseInputCameraController::Update(DeltaTime, Camera);

	CurrentTime += DeltaTime;

	if (CurrentTime >= Duration)
	{
		CurrentTime = Duration;
	}

	float SplinePercent = CurrentTime / Duration;

	glm::vec3 CameraPosition = SplinePath.Evaluate(SplinePercent);
	Camera->SetPosition(CameraPosition);
}
