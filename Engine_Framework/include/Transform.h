#ifndef Transform_h__
#define Transform_h__

#include <glm\glm.hpp>

struct FTransform
{
	FTransform(const glm::vec3& Position = glm::vec3(0.0f), const glm::vec3& Rotation = glm::vec3(0.0f), const glm::vec3& Scale = glm::vec3(1.0f)) : Position(Position), Rotation(Rotation), Scale(Scale) { }
	operator glm::mat4() const;

	glm::mat4 ToMatrix() const;

	glm::vec3 Position, Rotation, Scale;

	static const FTransform Identity;
};

#endif // Transform_h__
