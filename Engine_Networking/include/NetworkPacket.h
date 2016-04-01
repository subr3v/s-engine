#ifndef NetworkPacket_h__
#define NetworkPacket_h__

#include <vector>
#include <memory>
#include "Serializer.h"

struct FPacketHeader
{
	u8 PacketID;
	u16 PacketSize;

	void Serialize(FSerializer& Serializer);

	void DecodeFromMemory(const TVector<u8>& Memory);
};

class FNetworkPacket
{
public:
	FNetworkPacket();
	FNetworkPacket(u8 PacketID);
	~FNetworkPacket();

	void EncodeHeader();
	void Reserve(uint16 PacketSize);

	void* GetRawData();
	int GetRawDataSize();

	FPacketHeader GetHeader();
private:
	TVector<u8> PacketData;
	u8 PacketID;
	friend class FPacketWriter;
};

class FPacketReader : public FSerializer
{
public:
	FPacketReader(FNetworkPacket* Packet, bool bSkipHeader = true);

private:

};

class FPacketWriter : public FSerializer
{
public:
	FPacketWriter(FNetworkPacket* Packet, bool bSkipHeader = true);

private:

};

#endif // NetworkPacket_h__
