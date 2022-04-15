#pragma once

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16 port);

	auto GetASockAddr() -> SOCKADDR_IN& { return _sockAddr; }
	auto GetIpAddress() -> std::wstring;
	auto GetPort() -> uint16 { return ::ntohs(_sockAddr.sin_port); }

public:
	auto Ip2Address(const WCHAR* ip) -> IN_ADDR;

private:
	SOCKADDR_IN _sockAddr{};
};

