#pragma once

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16 port);

	auto GetSockAddr() -> SOCKADDR_IN&;
	auto GetIpAddress() -> std::wstring;
	auto GetPort() -> uint16;

public:
	auto Ip2Address(const WCHAR* ip) -> IN_ADDR;

private:
	SOCKADDR_IN _sockAddr{};
};

