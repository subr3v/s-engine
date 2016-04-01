#include "NetworkPacket.h"
#include "MemoryArchive.h"

FNetworkPacket::FNetworkPacket()
{

}

FNetworkPacket::FNetworkPacket(u8 PacketID) : PacketID(PacketID)
{
	PacketData.Resize(sizeof(FPacketHeader));
}

void FNetworkPacket::EncodeHeader()
{
	FPacketHeader Header;
	Header.PacketID = PacketID;
	Header.PacketSize = PacketData.Size();

	// Write header
	FPacketWriter PacketWriter(this, false);
	PacketWriter.SerializeObject(Header);
}

FNetworkPacket::~FNetworkPacket()
{

}

void* FNetworkPacket::GetRawData()
{
	return PacketData.Data();
}

int FNetworkPacket::GetRawDataSize()
{	
	return PacketData.Size();
}

FPacketHeader FNetworkPacket::GetHeader()
{
	FPacketReader Reader(this, false);
	FPacketHeader Header;
	Reader.SerializeObject(Header);
	return Header;
}

void FNetworkPacket::Reserve(uint16 PacketSize)
{
	PacketData.Resize(PacketSize);
}

void FPacketHeader::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeUInt8(PacketID);
	Serializer.SerializeUInt16(PacketSize);
}

void FPacketHeader::DecodeFromMemory(const TVector<u8>& Memory)
{
	FSerializer Serializer(new FMemoryReadArchive(Memory.Data()), true, true, true);
	Serializer.SerializeObject(*this);
}

FPacketReader::FPacketReader(FNetworkPacket* Packet, bool bSkipHeader) 
	: FSerializer(new FMemoryReadArchive(Packet->GetRawData(), (bSkipHeader ? sizeof(FPacketHeader) : 0)), true, true, true)
{

}

FPacketWriter::FPacketWriter(FNetworkPacket* Packet, bool bSkipHeader) 
	: FSerializer(new FMemoryBufferWriteArchive(Packet->PacketData, bSkipHeader ? sizeof(FPacketHeader) : 0), false, true, true)
{

}
