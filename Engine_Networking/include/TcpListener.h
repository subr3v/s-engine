#ifndef TcpListener_h__
#define TcpListener_h__

#include <winsock2.h>
#include "IpAddress.h"
#include "NetworkEventHandler.h"

class FNetworkEventHandler;

// Creates a socket and binds it to a port, waiting for connection and accepting them.
// Accepting a connection will yield a FConnection.
class FTcpListener : public IAsyncSocket
{
public:
	FTcpListener(FNetworkEventHandler* NetworkEventHandler);
	~FTcpListener();

	bool Bind(FIpAddress Address);
	bool Listen();

	class FAsyncConnection* AcceptConnection();

private:
	void OnAccept();

	FNetworkEventHandler* NetworkEventHandler;
	SOCKET listenerSocket;

	TVector<FAsyncConnection*> AcceptedConnections;

	virtual void OnMessage(ESocketMessageType MessageType, int ErrorCode) override;
	virtual SOCKET GetSocket() override;

	static void HandleNetworkError();
};

#endif // TcpListener_h__
