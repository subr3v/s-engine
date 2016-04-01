#include "AsyncConnection.h"
#include "NetworkPacket.h"

FAsyncConnection::FAsyncConnection(FNetworkEventHandler* EventHandler, SOCKET Socket, int BufferSize) : Socket(Socket), bIsConnectionValid(true),
ReadBuffer(BufferSize), WriteBuffer(BufferSize), EventHandler(EventHandler)
{
	EventHandler->RegisterSocket(this, ESocketMessageType::Close | ESocketMessageType::Read | ESocketMessageType::Write);

	bool bOption = true;
	setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bOption, sizeof(bool));
}

FAsyncConnection::~FAsyncConnection()
{
	EventHandler->UnregisterSocket(this);
}

void FAsyncConnection::OnRead()
{
	if (IsValid() == false)
	{
		// TODO: log error here
		return;
	}

	int LastError = 0;
	// Note the loop here -- we must keep reading until we get a WSAEWOULDBLOCK error.
	while (LastError == 0)
	{
		// Receive into whatever's left of the receive buffer.
		int ReceivedBytes = recv(Socket, (char*) ReadBuffer.GetCurrentPointerData(), ReadBuffer.GetSpaceLeft(), 0);
		if (ReceivedBytes == SOCKET_ERROR)
		{
			LastError = WSAGetLastError();
			// We might have a WSAEWOULDBLOCK or something else, but we'll handle that later.
			break;
		}
		else if (ReceivedBytes == 0 && ReadBuffer.GetSpaceLeft() > 0)
		{
			// The server closed the connection.
			bIsConnectionValid = false;
			break;
		}
		else if (ReceivedBytes == 0 && ReadBuffer.GetSpaceLeft() == 0)
		{
			// We ran out of local buffer space. wait till the client actually retrieves data.
			break;
		}
		else
		{
			ReadBuffer.IncreaseBytesCount(ReceivedBytes);
		}
	}

	if (IsValid() && LastError != 0 && LastError != WSAEWOULDBLOCK)
	{
		bIsConnectionValid = false;
		// Something wrong happened, try to recover from here if possible.
	}
}

void FAsyncConnection::OnWrite()
{
	if (IsValid() == false)
	{
		// TODO: log error here
		return;
	}

	int LastError = 0;
	while (WriteBuffer.GetTotalBytes() > 0)
	{
		// Send as much data as we can.
		int SentBytes = send(Socket, (char*)WriteBuffer.GetStartPointerData(), WriteBuffer.GetTotalBytes(), 0);
		if (SentBytes == SOCKET_ERROR)
		{
			LastError = WSAGetLastError();

			break;
		}
		else if (SentBytes == 0)
		{
			// The server closed the connection.
			bIsConnectionValid = false;
			break;
		}
		else
		{
			WriteBuffer.DecreaseByteCount(SentBytes);
		}
	}

	if (IsValid() && LastError != 0 && LastError != WSAEWOULDBLOCK)
	{
		bIsConnectionValid = false;
		// Something wrong happened, try to recover from here if possible.

	}
}

void FAsyncConnection::SendData(void* DataBuffer, u32 DataSize)
{
	if (WriteBuffer.GetSpaceLeft() < DataSize)
	{
		// TODO: Throw an error cause we ran out of write buffer space.
		return;
	}

	WriteBuffer.CopyData(DataBuffer, DataSize);

	// Start writing.
	OnWrite();
}

bool FAsyncConnection::IsValid()
{
	return bIsConnectionValid;
}

u32 FAsyncConnection::GetReceivedBytesCount()
{
	return ReadBuffer.GetTotalBytes();
}

bool FAsyncConnection::ReadData(void* DataBuffer, u32 DataSize)
{
	if (GetReceivedBytesCount() >= DataSize)
	{
		memcpy(DataBuffer, ReadBuffer.GetStartPointerData(), DataSize);
		ReadBuffer.DecreaseByteCount(DataSize);
		return true;
	}

	OnRead();

	return false;
}

bool FAsyncConnection::PeekData(void* DataBuffer, u32 DataSize)
{
	if (GetReceivedBytesCount() >= DataSize)
	{
		memcpy(DataBuffer, ReadBuffer.GetStartPointerData(), DataSize);
		return true;
	}

	OnRead();

	return false;
}

void FAsyncConnection::OnMessage(ESocketMessageType MessageType, int ErrorCode)
{
	if (ErrorCode != 0)
	{
		//TODO: Handle
		return;
	}

	if (MessageType == ESocketMessageType::Close)
	{
		bIsConnectionValid = false;
	}

	if (MessageType == ESocketMessageType::Read)
	{
		OnRead();
	}

	if (MessageType == ESocketMessageType::Write)
	{
		OnWrite();
	}
}

SOCKET FAsyncConnection::GetSocket()
{
	return Socket;
}

void FAsyncConnection::Close()
{
	closesocket(Socket);
	bIsConnectionValid = false;
}

FNetworkPacket* FAsyncConnection::TryReadPacket()
{
	TVector<u8> HeaderData(sizeof FPacketHeader);
	if (PeekData(HeaderData.Data(), sizeof(FPacketHeader)))
	{
		// Decode packet header
		FPacketHeader Header;
		Header.DecodeFromMemory(HeaderData);

		FNetworkPacket* Packet = new FNetworkPacket();
		Packet->Reserve(Header.PacketSize);
		ReadData(Packet->GetRawData(), Header.PacketSize);

		return Packet;
	}

	return nullptr;
}	

void FAsyncConnection::SendPacket(FNetworkPacket* Packet)
{
	Packet->EncodeHeader();
	SendData(Packet->GetRawData(), Packet->GetRawDataSize());
}
