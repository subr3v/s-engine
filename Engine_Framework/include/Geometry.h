#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <glm/glm.hpp>
#include "Vector.h"

struct FRay
{
	glm::vec3 Position;
	glm::vec3 Direction;
	float Magnitude;

	FRay(const glm::vec3& Position, const glm::vec3& Direction, float Magnitude) : Position(Position), Direction(Direction), Magnitude(Magnitude) { }

	void Draw(const glm::vec3& Colour);
};

struct FBoundingBox
{
	glm::vec3 Position;
	glm::vec3 Min;
	glm::vec3 Max;

	FBoundingBox() : Position(glm::vec3(0.0f)), Min(glm::vec3(FLT_MAX)), Max(glm::vec3(FLT_MIN)) { }

	void InflateFromVertices(const TVector<glm::vec3>& Vertices);

	bool CollidesWith(const FRay& Ray, float& Distance) const;
	bool CollidesWith(const FBoundingBox& Box) const;

	void Draw(const glm::vec3& Colour);
};

#endif // BoundingBox_h__
