#include "Spline.h"

void FSpline::AddPoint(const glm::vec3& Point)
{
	Points.Add(Point);
}

glm::vec3 FSpline::CatmullRomEquation(float t, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) const
{
	return	0.5f * ((2.0f * v1) +
		(-v0 + v2) * t +
		(2.0f * v0 - 5.0f * v1 + 4.0f * v2 - v3) * t * t +
		(-v0 + 3.0f * v1 - 3.0f * v2 + v3) * t * t * t);
}

glm::vec3 FSpline::Evaluate(float t) const
{
	float Delta = 1.0f / (float)Points.Size();
	
	// Find out starting index
	int Index = (int)(t / Delta);

	// Compute local control point indices
	int p0 = ClampIndex(Index - 1);
	int p1 = ClampIndex(Index);
	int p2 = ClampIndex(Index + 1);
	int p3 = ClampIndex(Index + 2);

	float LocalTime = (t - Delta * (float)Index) / Delta;
	// Interpolate
	return CatmullRomEquation(LocalTime, Points[p0], Points[p1], Points[p2], Points[p3]);
}

size_t FSpline::ClampIndex(size_t Index) const
{
	if (Index < 0)
		return 0;
	else if (Index >= Points.Size() - 1)
		return Points.Size() - 1;
	else
		return Index;
}

void FSpline::Draw(int Subdivision, const glm::vec3& Colour)
{
	// TODO: reimplement this.
	/*
	FScopedGLState Lighting(GL_LIGHTING, GL_FALSE);
	FScopedGLState Texture(GL_TEXTURE_2D, GL_FALSE);

	glBegin(GL_LINES);

	glColor3f(Colour.r, Colour.g, Colour.b);

	float CurrentT = 0.0f;
	float Step = 1.0f / Subdivision;

	for (int i = 0; i < Subdivision - 1; i++)
	{
		glm::vec3 v0, v1;
		v0 = Evaluate(CurrentT);
		v1 = Evaluate(CurrentT + Step);

		glVertex3f(v0.x, v0.y, v0.z);
		glVertex3f(v1.x, v1.y, v1.z);

		CurrentT += Step;
	}

	glEnd();
	*/
}
