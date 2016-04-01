#ifndef Connection_h__
#define Connection_h__

#include <WinSock2.h>
#include "NetworkBuffer.h"
#include "NetworkEventHandler.h"

class FNetworkPacket;

// Low level connection wrapper
class FAsyncConnection : public IAsyncSocket
{
public:
	FAsyncConnection(FNetworkEventHandler* EventHandler, SOCKET Socket, int BufferSize = 4096);
	~FAsyncConnection();

	void SendData(void* DataBuffer, u32 DataSize);

	u32 GetReceivedBytesCount();
	bool ReadData(void* DataBuffer, u32 DataSize);
	bool PeekData(void* DataBuffer, u32 DataSize);

	void SendPacket(FNetworkPacket* Packet);
	FNetworkPacket* TryReadPacket();

	void Close();

	bool IsValid();
private:
	void OnRead();
	void OnWrite();

	virtual void OnMessage(ESocketMessageType MessageType, int ErrorCode) override;
	virtual SOCKET GetSocket() override;

	FNetworkEventHandler* EventHandler;

	SOCKET Socket;

	FNetworkBuffer ReadBuffer;
	FNetworkBuffer WriteBuffer;

	bool bIsConnectionValid;
};

#endif // Connection_h__
