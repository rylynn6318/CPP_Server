#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();

public:
	auto SetNetAddress(NetAddress netAddress) -> void;
	auto GetNetAddress() -> NetAddress;
	auto GetSocket() -> SOCKET;

public:
	virtual auto GetHandle() -> HANDLE override;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) -> void override;

public:
	// TEMP
	char _recvBuffer[1000];

private:
	SOCKET _socket{ INVALID_SOCKET };
	NetAddress _netAddress{};
	Atomic<bool> _connected{ false };
};

