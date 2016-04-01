#ifndef NetworkEventHandler_h__
#define NetworkEventHandler_h__

#include <WinSock2.h>
#include "WindowsEventHandler.h"
#include "Vector.h"

enum class ESocketMessageType : char
{
	None = 0,
	Read = 1,
	Write = 2,
	Accept = 4,
	Connect = 8,
	Close = 16
};

inline ESocketMessageType operator|(ESocketMessageType A, ESocketMessageType B)
{
	return (ESocketMessageType)((char)A | (char)B);
}

inline bool operator&(ESocketMessageType A, ESocketMessageType B)
{
	return ((char)A & (char)B) != 0;
}

class IAsyncSocket
{
public:
	virtual void OnMessage(ESocketMessageType MessageType, int ErrorCode) = 0;
	virtual SOCKET GetSocket() = 0;
};


class FNetworkEventHandler : public IWindowsEventHandler
{
public:

	virtual void HandleMessage(MSG Message) override;

	void RegisterSocket(IAsyncSocket* Socket, ESocketMessageType MessageType);
	void UnregisterSocket(IAsyncSocket* Socket);

	static const int kSocketMessageID = WM_USER + 1;
private:

	static ESocketMessageType MessageTypeFromID(int ID);
	static int IDFromMessageType(ESocketMessageType MessageType);

	TVector<IAsyncSocket*> Sockets;
};

#endif // NetworkEventHandler_h__
