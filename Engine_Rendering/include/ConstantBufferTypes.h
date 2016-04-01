#ifndef ConstantBufferTypes_h__
#define ConstantBufferTypes_h__

#include <glm\glm.hpp>
#include <string>
#include "MemoryUtils.h"

#define alignas(size) __declspec( align(size) )

#pragma pack(push,4)

// Frequency: per object pass.
struct alignas(16) FInstanceBuffer : public FGraphicsAlignedBuffer
{
	FInstanceBuffer() { }
	FInstanceBuffer(const glm::mat4& WorldMatrix) { SetWorld(WorldMatrix); }

	void SetWorld(const glm::mat4& WorldMatrix)
	{
		this->WorldMatrix = WorldMatrix;
		this->InverseTransposeWorldMatrix = glm::transpose(glm::inverse(WorldMatrix));
	}

private:
	glm::mat4 WorldMatrix;
	glm::mat4 InverseTransposeWorldMatrix;

public:
	static FInstanceBuffer Identity;
	static ConstantString kBufferName;
};

struct alignas(16) FMaterialBuffer : public FGraphicsAlignedBuffer
{
	FMaterialBuffer() { }

	glm::vec3 Albedo;
	float Roughness;
	float RefractionIndex;
	glm::vec3 Padding;

	static ConstantString kBufferName;
};

#pragma pack(pop) 

struct FTextureNames
{
	static ConstantString kDiffuseMap;
	static ConstantString kNormalMap;
	static ConstantString kRoughnessMap;
	static ConstantString kRefractionMap;
};

#endif // ConstantBufferTypes_h__
