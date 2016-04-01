#ifndef BaseScene_h__
#define BaseScene_h__

#include "MaterialManager.h"
#include "DrawCallParameters.h"

struct FAmbientLight
{
	glm::vec3 Colour;
	float Intensity;

	FAmbientLight(const glm::vec3& Colour, float Intensity) : Colour(Colour), Intensity(Intensity) { }
	FAmbientLight() { }
};

struct FSpotlight
{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Colour;
	float Phi, Intensity, Radius;
	bool bCastShadow;
};

struct FPointLight
{
	glm::vec3 Position;
	glm::vec3 Colour;
	float Radius;
	float Intensity;
	bool bCastShadow;
};

struct FDirectionalLight
{
	glm::vec3 Direction;
	glm::vec3 Colour;
	float Intensity;
	bool bCastShadow;
};

class IScene
{
public:
	virtual void Draw(FDrawCall Params) = 0;
	virtual const TVector<FPointLight>& GetPointLights() const = 0;
	virtual const TVector<FDirectionalLight>& GetDirectionalLights() const = 0;
	virtual const TVector<FSpotlight>& GetSpotLights() const = 0;
	virtual const FAmbientLight& GetAmbientLight() const = 0;
};

#endif // BaseScene_h__
