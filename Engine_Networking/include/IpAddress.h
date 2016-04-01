#ifndef IpAddress_h__
#define IpAddress_h__

#include <winsock2.h>
#include <string>
#include "Core.hpp"

class FIpAddress
{
public:
	FIpAddress();
	FIpAddress(const std::string& AddressName, int Port, bool bIpV6 = false);

	const sockaddr* GetAddress() const;
	int32 GetAddressSize() const;
	std::string ToString() const;
	bool operator==(const FIpAddress& Other) const;
	
private:
	sockaddr* Address;
	int32 AddressSize;
};

#endif // IpAddress_h__
