#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
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

auto Service::SetSessionFactory(SessionFactory func)
{
	_sessionFactory = func;
}

auto Service::CreateSession() -> SessionRef
{
	SessionRef session = _sessionFactory();

	if (_iocpCore->Register(session) == false)
		return nullptr;
	
	return session;
}

auto Service::AddSession(SessionRef session) -> void
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

auto Service::ReleaseSession(SessionRef session) -> void
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

auto Service::GetIocpCore() -> IocpCoreRef&
{
	return _iocpCore;
}

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	:Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

auto ClientService::Start() -> bool
{
	// TODO
	return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount /*= 1*/)
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

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

auto ServerService::CloseService() -> void
{
	Service::CloseService();
}
