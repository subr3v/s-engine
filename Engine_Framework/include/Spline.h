#ifndef Spline_h__
#define Spline_h__

#include <glm/glm.hpp>
#include "Vector.h"

class FSpline
{
public:

	void AddPoint(const glm::vec3& Point);
	glm::vec3 Evaluate(float t) const;
	const TVector<glm::vec3>& GetPoints() const { return Points; }

	void Draw(int Subdivision, const glm::vec3& Colour);
private:
	size_t ClampIndex(size_t Index) const;
	glm::vec3 CatmullRomEquation(float t, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) const;

	TVector<glm::vec3> Points;
};

#endif // Spline_h__
