#include "NetworkEventHandler.h"
#include <algorithm>


void FNetworkEventHandler::HandleMessage(MSG Message)
{
	// We only handle socket messages here.
	if (Message.message != kSocketMessageID)
	{
		return;
	}

	SOCKET SocketHandle = (SOCKET)Message.wParam;
	long Error = WSAGETSELECTERROR(Message.lParam);
	int MessageType = WSAGETSELECTEVENT(Message.lParam);
	ESocketMessageType SocketMessageType = MessageTypeFromID(MessageType);

	// Pass through message to the appropriate socket.
	for (auto AsynSocket : Sockets)
	{
		if (AsynSocket->GetSocket() == SocketHandle)
		{
			AsynSocket->OnMessage(SocketMessageType, Error);
			break;
		}
	}
}

void FNetworkEventHandler::RegisterSocket(IAsyncSocket* Socket, ESocketMessageType MessageType)
{
	int Result = WSAAsyncSelect(Socket->GetSocket(), GetWindowHandle(), FNetworkEventHandler::kSocketMessageID, IDFromMessageType(MessageType));

	Sockets.Add(Socket);
}

void FNetworkEventHandler::UnregisterSocket(IAsyncSocket* Socket)
{
	Sockets.Remove(Socket);
}

ESocketMessageType FNetworkEventHandler::MessageTypeFromID(int ID)
{
	ESocketMessageType Type = ESocketMessageType::None;
	if (ID & FD_READ) { Type = Type | ESocketMessageType::Read; }
	if (ID & FD_WRITE) { Type = Type | ESocketMessageType::Write; }
	if (ID & FD_ACCEPT) { Type = Type | ESocketMessageType::Accept; }
	if (ID & FD_CONNECT) { Type = Type | ESocketMessageType::Connect; }
	if (ID & FD_CLOSE) { Type = Type | ESocketMessageType::Close; }
	return Type;
}

int FNetworkEventHandler::IDFromMessageType(ESocketMessageType MessageType)
{
	int ID = 0;
	if (MessageType & ESocketMessageType::Read) { ID = ID | FD_READ; }
	if (MessageType & ESocketMessageType::Write) { ID = ID | FD_WRITE; }
	if (MessageType & ESocketMessageType::Accept) { ID = ID | FD_ACCEPT; }
	if (MessageType & ESocketMessageType::Connect) { ID = ID | FD_CONNECT; }
	if (MessageType & ESocketMessageType::Close) { ID = ID | FD_CLOSE; }
	return ID;
}
