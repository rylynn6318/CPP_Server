#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;

class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	auto StartAccept(NetAddress netAddress)->bool;
	auto CloseSocket()->void;

public:
	virtual auto GetHandle()->HANDLE override;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0)->void override;

private:
	auto RegisterAccept(AcceptEvent* acceptEvent)->void;
	auto ProcessAccept(AcceptEvent* acceptEvent)->void;

protected:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
};

