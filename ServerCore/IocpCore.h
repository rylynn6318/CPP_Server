#pragma once

class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual auto GetHandle() -> HANDLE = 0;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) -> void = 0;
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	auto GetHandle() -> HANDLE { return _iocpHandle; }

	auto Register(std::shared_ptr<IocpObject> iocpObject) -> bool;
	auto Dispatch(uint32 timeoutMs = INFINITE) -> bool;

private:
	HANDLE _iocpHandle;
};
