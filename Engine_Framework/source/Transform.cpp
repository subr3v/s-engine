#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

FTransform::operator glm::mat4() const
{
	return ToMatrix();
}

glm::mat4 FTransform::ToMatrix() const
{
	return glm::scale(glm::translate(glm::mat4(1.0f), Position) * glm::yawPitchRoll(glm::radians(Rotation.y), glm::radians(Rotation.x), glm::radians(Rotation.z)), Scale);
}

const FTransform FTransform::Identity = FTransform();