#include "TcpListener.h"
#include "NetworkEventHandler.h"
#include "Window.h"
#include "AsyncConnection.h"

FTcpListener::FTcpListener(FNetworkEventHandler* NetworkEventHandler) : NetworkEventHandler(NetworkEventHandler)
{
	listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenerSocket == INVALID_SOCKET)
	{
		HandleNetworkError();
		//TODO: handle this error
	}
}

FTcpListener::~FTcpListener()
{
	NetworkEventHandler->UnregisterSocket(this);
	closesocket(listenerSocket);
}

bool FTcpListener::Bind(FIpAddress Address)
{
	int Result = bind(listenerSocket, (const sockaddr*)Address.GetAddress(), Address.GetAddressSize());
	if (Result != 0)
	{
		HandleNetworkError();
		// TODO: handle error
		return false;
	}

	// Put the socket into asynchronous mode and let the handler take care of windows messaging system.
	NetworkEventHandler->RegisterSocket(this, ESocketMessageType::Accept | ESocketMessageType::Close | ESocketMessageType::Connect | ESocketMessageType::Read);

	if (Result == SOCKET_ERROR)
	{
		HandleNetworkError();
		// TODO: Handle error.
		return false;
	}

	return true;
}

bool FTcpListener::Listen()
{
	int Result = listen(listenerSocket, 1);
	if (Result != 0)
	{
		HandleNetworkError();
		// TODO: Handle error.
		return false;
	}

	return true;
}

void FTcpListener::OnMessage(ESocketMessageType MessageType, int ErrorCode)
{
	if (ErrorCode != 0)
	{
		// Handle error:

		return;
	}

	switch (MessageType)
	{
		case ESocketMessageType::Accept:
			OnAccept();
			break;
	}
}

SOCKET FTcpListener::GetSocket()
{
	return listenerSocket;
}

void FTcpListener::HandleNetworkError()
{
	char ErrorBuffer[256];
	sprintf_s(ErrorBuffer, sizeof(ErrorBuffer), "Error: WSAGetLastError() = %d \n", WSAGetLastError());
	FMessageBox::ShowSimpleMessage(ErrorBuffer);
}

void FTcpListener::OnAccept()
{
	sockaddr_in ClientAddress;
	int ClientAddressSize = sizeof(ClientAddress);
	SOCKET ClientSocket = accept(listenerSocket, (sockaddr *)&ClientAddress, &ClientAddressSize);
	
	if (ClientSocket != INVALID_SOCKET)
	{
		FAsyncConnection* Connection = new FAsyncConnection(NetworkEventHandler, ClientSocket);
		AcceptedConnections.Add(Connection);
	}

	if (ClientSocket == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			// Handle error
			return;
		}
	}
}

class FAsyncConnection* FTcpListener::AcceptConnection()
{
	if (AcceptedConnections.Size() == 0)
	{
		return nullptr;
	}

	FAsyncConnection* LastConnection = AcceptedConnections.Last();
	AcceptedConnections.Pop();
	return LastConnection;
}
