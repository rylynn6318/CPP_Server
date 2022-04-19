#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Session.h"

Service::Service(ServiceType type, NetAddress address, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount)
	:_type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{
}

Service::~Service()
{
}

auto Service::CloseService() -> void
{
	// TODO
}

auto Service::CanStart() -> bool
{
	return _sessionFactory != nullptr;
}

auto Service::SetSessionFactory(std::function<std::shared_ptr<Session>(void)> func) -> void
{
	_sessionFactory = func;
}

auto Service::CreateSession() -> std::shared_ptr<Session>
{
	std::shared_ptr<Session> session = _sessionFactory();

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

auto Service::AddSession(std::shared_ptr<Session> session) -> void
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

auto Service::ReleaseSession(std::shared_ptr<Session> session) -> void
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

auto Service::GetCurrentSessionCount() -> int32
{
	return _sessionCount;
}

auto Service::GetMaxSessionCount() -> int32
{
	return _maxSessionCount;
}

auto Service::GetServiceType() -> ServiceType
{
	return _type;
}

auto Service::GetNetAddress() -> NetAddress
{
	return _netAddress;
}

auto Service::GetIocpCore() -> std::shared_ptr<IocpCore>&
{
	return _iocpCore;
}

ClientService::ClientService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount)
	:Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

auto ClientService::Start() -> bool
{
	// TODO
	return true;
}

ServerService::ServerService(NetAddress address, std::shared_ptr<IocpCore> core, std::function<std::shared_ptr<Session>(void)> factory, int32 maxSessionCount /*= 1*/)
	:Service(ServiceType::Server, address, core, factory, maxSessionCount)
{

}

auto ServerService::Start() -> bool
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	std::shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

auto ServerService::CloseService() -> void
{
	Service::CloseService();
}
