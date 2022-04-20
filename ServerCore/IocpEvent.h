#pragma once
#include "IocpCore.h"

class Session;

enum class EventType :uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, 0 byte recv
	Recv,
	Send
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	auto Init() -> void;

public:
	EventType eventType;
	std::shared_ptr<IocpObject> owner;
};

class ConnectEvent :public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

class DisonnectEvent :public IocpEvent
{
public:
	DisonnectEvent() : IocpEvent(EventType::Disconnect) {}
};

class AcceptEvent :public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	std::shared_ptr<Session> session{ nullptr };
};

class RecvEvent :public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

class SendEvent :public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	// TEMP
	std::vector<BYTE> buffer;
};

