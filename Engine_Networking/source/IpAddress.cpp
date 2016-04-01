#include "IpAddress.h"
#include <ws2tcpip.h>

FIpAddress::FIpAddress(const std::string& AddressName, int Port, bool bIpV6)
{
	if (bIpV6)
	{
		sockaddr_in6* AddressInstance = (sockaddr_in6*)malloc(sizeof(sockaddr_in6));
		memset(AddressInstance, 0, sizeof(sockaddr_in6));	
		AddressInstance->sin6_port = htons(Port);
		AddressInstance->sin6_family = AF_INET6;
		inet_pton(AF_INET6, AddressName.c_str(), &AddressInstance->sin6_addr);
		Address = (sockaddr*)AddressInstance;
		AddressSize = sizeof(sockaddr_in6);
	}
	else
	{
		sockaddr_in* AddressInstance = (sockaddr_in*)malloc(sizeof(sockaddr_in));
		AddressInstance->sin_family = AF_INET;
		AddressInstance->sin_port = htons(Port);
		inet_pton(AF_INET, AddressName.c_str(), &AddressInstance->sin_addr);
		Address = (sockaddr*)AddressInstance;
		AddressSize = sizeof(sockaddr_in);
	}
}

FIpAddress::FIpAddress()
{
	free(Address);
	Address = nullptr;
}

std::string FIpAddress::ToString() const
{
	DWORD AddressSize = 0;
	WSAAddressToString((LPSOCKADDR)&Address, sizeof(sockaddr_in), nullptr, nullptr, &AddressSize);
	std::string Result;
	Result.resize(AddressSize);
	WSAAddressToString((LPSOCKADDR)&Address, sizeof(sockaddr_in), nullptr, (LPSTR)Result.data(), &AddressSize);
	return Result;
}

bool FIpAddress::operator==(const FIpAddress& Other) const
{
	return memcmp(&Address, &Other.Address, sizeof(sockaddr_in)) == 0;
}

const sockaddr* FIpAddress::GetAddress() const
{
	return Address;
}

int32 FIpAddress::GetAddressSize() const
{
	return AddressSize;
}
