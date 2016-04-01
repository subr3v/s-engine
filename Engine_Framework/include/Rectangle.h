#ifndef Rectangle_h__
#define Rectangle_h__

#include <glm/glm.hpp>

struct FRectangle
{
	FRectangle();
	FRectangle(float X, float Y, float Width, float Height);

	bool CollidesWith(const FRectangle& Other) const;
	FRectangle Translate(float X, float Y) const;
	float GetWidth() const;
	float GetHeight() const;

	glm::vec2 GetPosition() const;

private:
	float Left, Right, Top, Bottom;
};

#endif // Rectangle_h__
