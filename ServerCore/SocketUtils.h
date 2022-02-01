#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

public:
	static auto Init()->void;
	static auto Clear()->void;

	static auto BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)->bool;
	static auto CreateSocket()->SOCKET;

	static auto SetLinger(SOCKET socket, uint16 onoff, uint16 linger)->bool;
	static auto SetReuseAddress(SOCKET socket, bool flag)->bool;
	static auto SetRecvBufferSize(SOCKET socket, int32 size)->bool;
	static auto SetSendBufferSize(SOCKET socket, int32 size)->bool;
	static auto SetTcpNoDelay(SOCKET socket, bool flag)->bool;

	// ListenSocket의 특성을 ClientSocket에 그대로 적용
	static auto SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static auto Bind(SOCKET socket, NetAddress netAddr)->bool;
	static auto BindAnyAddress(SOCKET socket, uint16 port)->bool;
	static auto Listen(SOCKET socket, int32 backlog = SOMAXCONN)->bool;
	static auto Close(SOCKET& socket)->void;
};

template<typename T>
static inline auto SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)->bool
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}

