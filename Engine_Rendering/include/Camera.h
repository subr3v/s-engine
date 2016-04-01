#ifndef Camera_h__
#define Camera_h__

#include <glm/glm.hpp>
#include <d3d11.h>
#include "Viewport.h"

class FCamera
{
public:
	FCamera(const FViewport& Viewport = FViewport());
	~FCamera();

	void SetPosition(const glm::vec3& Position) { this->Position = Position; MarkDirty(); }
	void SetRotation(const glm::vec3& Rotation) { this->Rotation = Rotation; MarkDirty(); }
	void SetViewport(const FViewport& Viewport) { this->Viewport = Viewport; }

	glm::vec3 GetRotation() const { return Rotation; }
	glm::vec3 GetPosition() const { return Position; }

	float GetYaw() const { return Rotation.y; }
	float GetPitch() const { return Rotation.x; }
	float GetRoll() const { return Rotation.z; }

	void SetYaw(float Yaw);
	void SetPitch(float Pitch);
	void SetRoll(float Roll);

	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;

	glm::mat4 GetViewMatrix() const { CheckDirty(); return ViewMatrix; }
	glm::mat4 GetProjectionMatrix() const { return ProjectionMatrix; }

	//TODO: Put this back when geometry/math module is implemented.
	//FRay GetMouseRay(int MouseX, int MouseY) const;

	void SetPerspective(float Fov, float Width, float Height, float Near, float Far);
	void SetOrthogonal(float Left, float Right, float Top, float Bottom, float Near, float Far);

	const FViewport& GetViewport() const { return Viewport; }
private:
	void MarkDirty() { bRecalculateViewMatrix = true; }
	void CheckDirty() const;

	// Allowing lazy updating of camera view/projection matrix.
	mutable bool bRecalculateViewMatrix;
	mutable glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	FViewport Viewport;

	glm::vec3 Position;
	glm::vec3 Rotation;
};

#endif // Camera_h__
