#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

auto SocketUtils::Init() -> void
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	// 런타임에 주소 얻어오는 API
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
}

auto SocketUtils::Clear() -> void
{
	::WSACleanup();
}

auto SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn) -> bool
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

auto SocketUtils::CreateSocket() -> SOCKET
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

auto SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger) -> bool
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;

	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

auto SocketUtils::SetReuseAddress(SOCKET socket, bool flag) -> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

auto SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size) -> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

auto SocketUtils::SetSendBufferSize(SOCKET socket, int32 size) -> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

auto SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag) -> bool
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

auto SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

auto SocketUtils::Bind(SOCKET socket, NetAddress netAddr) -> bool
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetASockAddr()), sizeof(SOCKADDR_IN));
}

auto SocketUtils::BindAnyAddress(SOCKET socket, uint16 port) -> bool
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

auto SocketUtils::Listen(SOCKET socket, int32 backlog) -> bool
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

auto SocketUtils::Close(SOCKET& socket) -> void
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}
