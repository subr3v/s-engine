#include "Geometry.h"

#include "Mesh.h"
#include <gl/GL.h>
#include "GraphicsContext.h"
#include <algorithm>

using namespace std;

bool FBoundingBox::CollidesWith(const FRay& Ray, float& Distance) const
{
	// r.dir is unit direction vector of ray
	glm::vec3 DirInverse = 1.0f / Ray.Direction;

	glm::vec3 MinVector = Position + Min;
	glm::vec3 MaxVector = Position + Max;

	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray

	float t1 = (MinVector.x - Ray.Position.x) * DirInverse.x;
	float t2 = (MaxVector.x - Ray.Position.x) * DirInverse.x;
	float t3 = (MinVector.y - Ray.Position.y) * DirInverse.y;
	float t4 = (MaxVector.y - Ray.Position.y) * DirInverse.y;
	float t5 = (MinVector.z - Ray.Position.z) * DirInverse.z;
	float t6 = (MaxVector.z - Ray.Position.z) * DirInverse.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	float t = 0.0f;

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;

	Distance = Ray.Magnitude * t;
	return true;
}

bool FBoundingBox::CollidesWith(const FBoundingBox& Other) const
{
	glm::vec3 HalfSizeA = (Max - Min) * 0.5f;
	glm::vec3 HalfSizeB = (Other.Max - Other.Min) * 0.5f;

	glm::vec3 CenterA = Min + HalfSizeA + Position;
	glm::vec3 CenterB = Other.Min + HalfSizeB + Other.Position;

	glm::vec3 PositionDifference = glm::abs(CenterA - CenterB);
	glm::vec3 SizeSum = HalfSizeA + HalfSizeB;

	bool x = PositionDifference.x < SizeSum.x;
	bool y = PositionDifference.y < SizeSum.y;
	bool z = PositionDifference.z < SizeSum.z;

	return x && y && z;
}

void FBoundingBox::Draw(const glm::vec3& Colour)
{
	// TODO: Replace this somehow, might need a batched renderer.
	/*
	FScopedGLState Texture(GL_TEXTURE_2D, GL_FALSE);
	FScopedGLState Lighting(GL_LIGHTING, GL_FALSE);

	glPushMatrix();
	glTranslatef(Position.x, Position.y, Position.z);

	glBegin(GL_LINES);

	glColor3f(Colour.r, Colour.g, Colour.b);

	glVertex3f(Min.x, Min.y, Min.z);
	glVertex3f(Max.x, Min.y, Min.z);

	glVertex3f(Min.x, Max.y, Min.z);
	glVertex3f(Max.x, Max.y, Min.z);

	glVertex3f(Min.x, Min.y, Max.z);
	glVertex3f(Max.x, Min.y, Max.z);

	glVertex3f(Min.x, Max.y, Max.z);
	glVertex3f(Max.x, Max.y, Max.z);

	glVertex3f(Min.x, Min.y, Min.z);
	glVertex3f(Min.x, Max.y, Min.z);

	glVertex3f(Max.x, Min.y, Min.z);
	glVertex3f(Max.x, Max.y, Min.z);

	glVertex3f(Min.x, Min.y, Max.z);
	glVertex3f(Min.x, Max.y, Max.z);

	glVertex3f(Max.x, Min.y, Max.z);
	glVertex3f(Max.x, Max.y, Max.z);

	glVertex3f(Min.x, Min.y, Min.z);
	glVertex3f(Min.x, Min.y, Max.z);

	glVertex3f(Max.x, Min.y, Min.z);
	glVertex3f(Max.x, Min.y, Max.z);

	glVertex3f(Min.x, Max.y, Min.z);
	glVertex3f(Min.x, Max.y, Max.z);

	glVertex3f(Max.x, Max.y, Min.z);
	glVertex3f(Max.x, Max.y, Max.z);

	glEnd();

	glPopMatrix();
	*/
}

void FBoundingBox::InflateFromVertices(const TVector<glm::vec3>& Vertices)
{
	for (size_t i = 0; i < Vertices.Size(); i++)
	{
		const glm::vec3& Vertex = Vertices[i];
		
		if (Vertex.x < Min.x)
			Min.x = Vertex.x;
		if (Vertex.x > Max.x)
			Max.x = Vertex.x;

		if (Vertex.y < Min.y)
			Min.y = Vertex.y;
		if (Vertex.y > Max.y)
			Max.y = Vertex.y;

		if (Vertex.z < Min.z)
			Min.z = Vertex.z;
		if (Vertex.z > Max.z)
			Max.z = Vertex.z;
	}
}

void FRay::Draw(const glm::vec3& Colour)
{
	// TODO: Replace this somehow, might need a batched renderer.

	/*
	FScopedGLState Lighting(GL_LIGHTING, GL_FALSE);
	FScopedGLState Texture(GL_TEXTURE_2D, GL_FALSE);

	glPushMatrix();
	glTranslatef(Position.x, Position.y, Position.z);
	
	glBegin(GL_LINES);

	glColor3f(Colour.r, Colour.g, Colour.b);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(Direction.x  * Magnitude,
			   Direction.y * Magnitude, 
			   Direction.z * Magnitude);

	glEnd();

	glPopMatrix();
	*/
}
