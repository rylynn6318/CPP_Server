#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = std::function<SessionRef(void)>;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual auto Start()->bool abstract;
	virtual auto CloseService()->void;

public:
	auto CanStart()->bool;
	auto SetSessionFactory(SessionFactory func);
	auto CreateSession()->SessionRef;
	auto AddSession(SessionRef session)->void;
	auto ReleaseSession(SessionRef session)->void;
	auto GetCurrentSessionCount()->int32;
	auto GetMaxSessionCount()->int32;
	auto GetServiceType()->ServiceType;
	auto GetNetAddress()->NetAddress;
	auto GetIocpCore()->IocpCoreRef&;

protected:
	USE_LOCK;

	ServiceType _type;
	NetAddress _netAddress{};
	IocpCoreRef _iocpCore;

	Set<SessionRef> _sessions;
	int32 _sessionCount{ 0 };
	int32 _maxSessionCount{ 0 };
	SessionFactory _sessionFactory;
};


class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() = default;

	virtual auto Start()->bool override;
};

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() = default;

	virtual auto Start()->bool override;
	virtual auto CloseService()->void;

private:
	ListenerRef _listener = nullptr;
};
