#ifndef TcpClient_h__
#define TcpClient_h__

#include "NetworkEventHandler.h"
#include "IpAddress.h"

class FAsyncConnection;

// Creates a socket and binds it to a random port, tries to connect to tcp listener and when the connection is successful it will
// yield a FConnection.
class FTcpClient : public IAsyncSocket
{
public:
	FTcpClient(FNetworkEventHandler* NetworkEventHandler, bool bIpV6 = false);
	~FTcpClient();

	bool Connect(FIpAddress Address);
	FAsyncConnection* GetConnection();

private:
	virtual void OnMessage(ESocketMessageType MessageType, int ErrorCode) override;
	virtual SOCKET GetSocket() override;

	FNetworkEventHandler* NetworkEventHandler;
	FAsyncConnection* Connection = nullptr; 
	SOCKET clientSocket;
};

#endif // TcpClient_h__
