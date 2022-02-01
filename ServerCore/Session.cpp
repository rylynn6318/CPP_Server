#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

auto Session::SetNetAddress(NetAddress netAddress) -> void
{
	_netAddress = netAddress;
}

auto Session::GetNetAddress() -> NetAddress
{
	return _netAddress;
}

auto Session::GetSocket() -> SOCKET
{
	return _socket;
}

auto Session::GetHandle() -> HANDLE
{
	return reinterpret_cast<HANDLE>(_socket);
}

auto Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) -> void
{
	// TODO
}
