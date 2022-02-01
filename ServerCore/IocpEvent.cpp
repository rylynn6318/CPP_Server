#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type) : _type(type)
{
	Init();
}

auto IocpEvent::Init() -> void
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
