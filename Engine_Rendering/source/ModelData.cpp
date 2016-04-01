#include "ModelData.h"
#include "MemoryArchive.h"

void FModelData::Serialize(FSerializer& Serializer)
{
	int32 Flag;
	Serializer.SerializeInt32(VertexCount);
	Serializer.SerializeInt32(Flag);

	ChannelFlag = (EMeshChannelType)Flag;

	Serializer.SerializeObjectArray(Indices);
	Serializer.SerializeObjectArray(MeshData);
	Serializer.SerializeObjectArray(Materials);

	Serializer.SerializeObjectArray(PositionChannel.Elements);
	Serializer.SerializeObjectArray(ColourChannel.Elements);
	Serializer.SerializeObjectArray(NormalChannel.Elements);
	Serializer.SerializeObjectArray(TangentChannel.Elements);
	Serializer.SerializeObjectArray(BiTangentChannel.Elements);
	Serializer.SerializeObjectArray(DiffuseUVChannel.Elements);
	Serializer.SerializeObjectArray(NormalUVChannel.Elements);
	Serializer.SerializeObjectArray(BumpUVChannel.Elements);
	Serializer.SerializeObjectArray(SkinChannel.Elements);
}

void FModelData::ResizeChannels(s32 VertexCount)
{
	this->VertexCount = VertexCount;
	
	if (ChannelFlag & EMeshChannelType::Position)
		PositionChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::Colour)
		ColourChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::Normal)
		NormalChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::DiffuseUV)
		DiffuseUVChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::NormalUV)
		NormalUVChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::BumpUV)
		BumpUVChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::Tangent)
		TangentChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::BiTangent)
		BiTangentChannel.Elements.Resize(VertexCount);

	if (ChannelFlag & EMeshChannelType::Skin)
		SkinChannel.Elements.Resize(VertexCount);
}

void FMeshData::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeInt16(MaterialIndex);
	Serializer.SerializeInt32(IndexStart);
	Serializer.SerializeInt32(IndexCount);
	Serializer.SerializeInt32(BaseVertex);
}

void FMaterialData::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeString(DiffuseTexturePath);
	Serializer.SerializeString(NormalTexturePath);
	Serializer.SerializeString(RoughnessTexturePath);
	Serializer.SerializeString(RefractionTexturePath);
	Serializer.SerializeString(OpacityTexturePath);

	Serializer.SerializeObject(DiffuseColour);
	Serializer.SerializeObject(SpecularColour);
	Serializer.SerializeObject(Roughness);
	Serializer.SerializeObject(Refraction);
}

void FSkinData::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeObject(BoneIndices);
	Serializer.SerializeObject(Weights);
}
