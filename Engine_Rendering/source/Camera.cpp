#include "Camera.h"

#include <gl/GLU.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.inl"
#include "glm/gtx/transform.inl"

FCamera::FCamera(const FViewport& Viewport) : Viewport(Viewport), Position(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f)
{
	// Default perspective for backward compatibility.
	SetPerspective(60.0f, (float)Viewport.Width, (float)Viewport.Height, 1.0f, 5000.0f);
}

FCamera::~FCamera()
{
}

glm::vec3 FCamera::GetForwardVector() const
{
	float Yaw = glm::radians(GetYaw());
	float Roll = glm::radians(GetRoll());
	float Pitch = glm::radians(GetPitch());

	float cosY = cosf(Yaw);
	float cosP = cosf(Pitch);
	float cosR = cosf(Roll);
	float sinY = sinf(Yaw);
	float sinP = sinf(Pitch);
	float sinR = sinf(Roll);

	return glm::normalize(glm::vec3(sinY * cosP, sinP, cosP * -cosY));
}

glm::vec3 FCamera::GetRightVector() const
{
	return glm::cross(GetForwardVector(), GetUpVector());
}

glm::vec3 FCamera::GetUpVector() const
{
	float Yaw = glm::radians(GetYaw());
	float Roll = glm::radians(GetRoll());
	float Pitch = glm::radians(GetPitch());

	float cosY = cosf(Yaw);
	float cosP = cosf(Pitch);
	float cosR = cosf(Roll);
	float sinY = sinf(Yaw);
	float sinP = sinf(Pitch);
	float sinR = sinf(Roll);

	return glm::vec3(-cosY * sinR - sinY * sinP * cosR, cosP * cosR, -sinY * sinR - sinP * cosR * -cosY);
}

void FCamera::CheckDirty() const
{
	if (bRecalculateViewMatrix)
	{
		glm::vec3 Up = GetUpVector();
		glm::vec3 Forward = Position + GetForwardVector();
		ViewMatrix = glm::lookAt(Position, Forward, Up);
		bRecalculateViewMatrix = false;
	}
}

/*
FRay FCamera::GetMouseRay(int MouseX, int MouseY) const
{	
	glm::vec3 MouseRay;
	glm::vec2 MousePosition = glm::vec2(MouseX, MouseY);
	glm::vec4 ViewportVector(Viewport.X, Viewport.Y, Viewport.Width, Viewport.Height);

	MousePosition.y = Viewport.Height - MousePosition.y;

	MouseRay = glm::unProject(glm::vec3(MousePosition, 1.0f), glm::mat4(1.0f), ProjectionMatrix, ViewportVector);

	const float kRayLength = 100000.0f;

	glm::vec3 RayDirection = glm::normalize(MouseRay - Position);
	return FRay(Position, RayDirection, kRayLength);
}
*/

void FCamera::SetPerspective(float Fov, float Width, float Height, float Near, float Far)
{
	this->ProjectionMatrix = glm::perspectiveFov(glm::radians(Fov), Width, Height, Near, Far);
}

void FCamera::SetOrthogonal(float Left, float Right, float Top, float Bottom, float Near, float Far)
{
	this->ProjectionMatrix = glm::ortho(Left, Right, Bottom, Top, Near, Far);
}

void FCamera::SetYaw(float Yaw)
{
	if (Yaw > 360)
	{
		Yaw = 0;
	}
	if (Yaw < -360)
	{
		Yaw = 0;
	}

	Rotation.y = Yaw; MarkDirty();
}

void FCamera::SetPitch(float Pitch)
{
	if (Pitch > 89)
	{
		Pitch = 89;
	}
	if (Pitch < -89)
	{
		Pitch = -89;
	}

	Rotation.x = Pitch; MarkDirty();
}

void FCamera::SetRoll(float Roll)
{
	Rotation.z = Roll; MarkDirty();
}

