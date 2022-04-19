#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

NetAddress::NetAddress(std::wstring ip, uint16 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

auto NetAddress::GetSockAddr() -> SOCKADDR_IN&
{
	return _sockAddr;
}

auto NetAddress::GetIpAddress() -> std::wstring
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len32(buffer));
	return std::wstring(buffer);
}

auto NetAddress::GetPort() -> uint16
{
	return ::ntohs(_sockAddr.sin_port);
}

auto NetAddress::Ip2Address(const WCHAR* ip) -> IN_ADDR
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
