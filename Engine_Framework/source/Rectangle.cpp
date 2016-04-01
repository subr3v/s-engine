#include "Rectangle.h"

FRectangle::FRectangle(float X, float Y, float Width, float Height)
{
	Left = X;
	Right = X + Width;
	Top = Y;
	Bottom = Y + Height;
}

FRectangle::FRectangle()
{
	Left = Right = Top = Bottom = 0.0f;
}

bool FRectangle::CollidesWith(const FRectangle& Other) const
{
	bool Collides = true;

	if (Right < Other.Left || Left > Other.Right || Bottom < Other.Top || Top > Other.Bottom)
		return false;

	return Collides;
}

FRectangle FRectangle::Translate(float X, float Y) const
{
	return FRectangle(this->Left + X, this->Top + Y, GetWidth(), GetHeight());
}

float FRectangle::GetHeight() const
{
	return Bottom - Top;
}

float FRectangle::GetWidth() const
{
	return Right - Left;
}

glm::vec2 FRectangle::GetPosition() const
{
	return glm::vec2(Left, Top);
}
