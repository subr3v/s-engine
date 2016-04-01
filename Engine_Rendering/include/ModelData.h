#ifndef IntermediateModelData_h__
#define IntermediateModelData_h__

#include <cstdint>
#include "Vector.h"
#include <glm\glm.hpp>
#include "Serializer.h"

enum class EMeshChannelType : int32
{
	Position = 1,
	Colour = 2,
	Normal = 4,
	DiffuseUV = 8,
	NormalUV = 16,
	BumpUV = 32,
	Skin = 64,
	Tangent = 128,
	BiTangent = 256,
};

inline bool operator &(EMeshChannelType lhs, EMeshChannelType rhs)
{
	return (static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs)) != 0;
}

inline EMeshChannelType operator |(EMeshChannelType lhs, EMeshChannelType rhs)
{
	return (EMeshChannelType)((static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs)));
}

inline EMeshChannelType& operator |=(EMeshChannelType& lhs, EMeshChannelType rhs)
{
	lhs = (EMeshChannelType)((static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs)));
	return lhs;
}

struct FSkinData
{
	glm::ivec4 BoneIndices;
	glm::vec4 Weights;

	void Serialize(FSerializer& Serializer);
};

template<typename T>
struct FMeshChannel
{
	TVector<T> Elements;
};

struct FMeshData
{
	s16 MaterialIndex;
	s32 IndexStart;
	s32 IndexCount;
	s32 BaseVertex;

	void Serialize(FSerializer& Serializer);
};

struct FMaterialData
{
	std::string DiffuseTexturePath;
	std::string NormalTexturePath;
	std::string RoughnessTexturePath;
	std::string RefractionTexturePath;
	std::string OpacityTexturePath;

	glm::vec3 DiffuseColour = glm::vec3(1.0f);
	glm::vec3 SpecularColour = glm::vec3(1.0f);
	float Roughness = 1.0;
	float Refraction = 1.0;

	void Serialize(FSerializer& Serializer);
};

struct FModelData
{
	s32 VertexCount;
	EMeshChannelType ChannelFlag;

	TVector<u32> Indices;

	FMeshChannel<glm::vec3> PositionChannel;
	FMeshChannel<glm::vec3> ColourChannel;
	FMeshChannel<glm::vec3> NormalChannel;
	FMeshChannel<glm::vec3> TangentChannel;
	FMeshChannel<glm::vec3> BiTangentChannel;
	FMeshChannel<glm::vec2> DiffuseUVChannel;
	FMeshChannel<glm::vec2> NormalUVChannel;
	FMeshChannel<glm::vec2> BumpUVChannel;
	FMeshChannel<FSkinData> SkinChannel;

	TVector<FMaterialData> Materials;
	TVector<FMeshData> MeshData;

	void ResizeChannels(s32 VertexCount);
	void Serialize(FSerializer& Serializer);
};


#endif // IntermediateModelData_h__
