#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum { BUFFER_SIZE = 0x10000 };

public:
	Session();
	virtual ~Session();

public:
	auto Send(std::shared_ptr<SendBuffer> sendBuffer) -> void;
	auto Connect() -> bool;
	auto Disconnect(const WCHAR* cause) -> void;
	auto GetService() -> std::shared_ptr<Service>;
	auto SetService(std::shared_ptr<Service> service) -> void;

public:
	auto SetNetAddress(NetAddress netAddress) -> void;
	auto GetNetAddress() -> NetAddress;
	auto GetSocket() -> SOCKET;
	auto IsConnected() -> bool;
	auto GetSession() -> std::shared_ptr<Session>;

private:
	virtual auto GetHandle() -> HANDLE override;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) -> void override;

private:
	// 전송
	auto RegisterConnect() -> bool;
	auto RegisterDisconnect() -> bool;
	auto RegisterRecv() -> void;
	auto RegisterSend() -> void;
	auto ProcessConnect() -> void;
	auto ProcessDisconnect() -> void;
	auto ProcessRecv(int32 numOfBytes) -> void;
	auto ProcessSend(int32 numOfBytes) -> void;
	auto HandleError(int32 errorCode) -> void;

protected:
	// 컨텐츠에서 오버로딩
	virtual auto OnConnected() -> void {}
	virtual auto OnRecv(BYTE* buffer, int32 len) -> int32 { return len; }
	virtual auto OnSend(int32 len) -> void {}
	virtual auto OnDisconnected() -> void {}

private:
	std::weak_ptr<Service> _service;
	SOCKET _socket{ INVALID_SOCKET };
	NetAddress _netAddress{};
	Atomic<bool> _connected{ false };

private:
	USE_LOCK;

	// 수신 관련
	RecvBuffer _recvBuffer;

	// 송신 관련
	Queue<std::shared_ptr<SendBuffer>> _sendQueue;
	Atomic<bool> _sendRegisterd{ false };

private:
	// IocpEvent 재사용

	DisonnectEvent _disconnectEvent;
	ConnectEvent _connectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	auto GetPacketSession() -> std::shared_ptr<PacketSession>;

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) final;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) = 0;
	
};

