#pragma once

class IocpObject
{
public:
	virtual auto GetHandle()->HANDLE abstract;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0)->void abstract;
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	auto GetHandle()->HANDLE { return _iocpHandle; }

	auto Register(class IocpObject* iocpObject)->bool;
	auto Dispatch(uint32 timeoutMs = INFINITE)->bool;

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;

