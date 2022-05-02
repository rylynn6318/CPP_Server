#pragma once
#include "NetAddress.h"
#include <functional>

class Session;
class IocpCore;
class Listener;

enum class ServiceType : uint8
{
	Server,
	Client
};

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual auto Start() -> bool = 0;
	virtual auto CloseService() -> void;

public:
	auto CanStart() -> bool;
	auto SetSessionFactory(std::function<std::shared_ptr<Session>(void)> func) -> void;
	auto CreateSession() -> std::shared_ptr<Session>;

	auto BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void;

	auto AddSession(std::shared_ptr<Session> session) -> void;
	auto ReleaseSession(std::shared_ptr<Session> session) -> void;
	auto GetCurrentSessionCount() -> int32;
	auto GetMaxSessionCount() -> int32;
	auto GetServiceType() -> ServiceType;
	auto GetNetAddress() -> NetAddress;
	auto GetIocpCore() -> std::shared_ptr<IocpCore>&;

protected:
	USE_LOCK;

	ServiceType _type;
	NetAddress _netAddress{};
	std::shared_ptr<IocpCore> _iocpCore;

	Set<std::shared_ptr<Session>> _sessions;
	int32 _sessionCount{ 0 };
	int32 _maxSessionCount{ 0 };
	std::function<std::shared_ptr<Session>(void)> _sessionFactory;
};


class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount = 1);
	virtual ~ClientService() = default;

	virtual auto Start() -> bool override;
};

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount = 1);
	virtual ~ServerService() = default;

	virtual auto Start() -> bool override;
	virtual auto CloseService() -> void;

private:
	std::shared_ptr<Listener> _listener{ nullptr };
};
