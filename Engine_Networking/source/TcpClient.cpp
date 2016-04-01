#include "TcpClient.h"
#include "AsyncConnection.h"


FTcpClient::FTcpClient(FNetworkEventHandler* NetworkEventHandler, bool bIpV6) : NetworkEventHandler(NetworkEventHandler)
{
	clientSocket = socket(bIpV6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		//TODO: handle this error
	}

}

FTcpClient::~FTcpClient()
{
	NetworkEventHandler->UnregisterSocket(this);
	closesocket(clientSocket);

	delete Connection;
	Connection = nullptr;
}

bool FTcpClient::Connect(FIpAddress Address)
{
	NetworkEventHandler->RegisterSocket(this, ESocketMessageType::Connect | ESocketMessageType::Close);

	int Result = connect(clientSocket, (const sockaddr*)Address.GetAddress(), Address.GetAddressSize());
	if (Result == -1)
	{
		int Error = WSAGetLastError();
		if (Error != WSAEWOULDBLOCK)
		{
			// Handle error.
			return false;
		}
	}

	return true;
}

FAsyncConnection* FTcpClient::GetConnection()
{
	return Connection;
}

void FTcpClient::OnMessage(ESocketMessageType MessageType, int ErrorCode)
{
	if (ErrorCode == 0 && MessageType == ESocketMessageType::Connect)
	{
		Connection = new FAsyncConnection(NetworkEventHandler, clientSocket);
	}
}

SOCKET FTcpClient::GetSocket()
{
	return clientSocket;
}
