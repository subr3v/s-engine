#ifndef MaterialsData_h__
#define MaterialsData_h__

struct FRenderFrameData
{
	float FrameTime;
	float DeltaTime;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::vec3 CameraPosition;

	u32 PointLightCount = 0;
	u32 SpotLightCount = 0;

	struct FViewport* SpriteViewport = nullptr;
};

#endif // MaterialsData_h__
