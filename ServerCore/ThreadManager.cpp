#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	InitTLS();

}

ThreadManager::~ThreadManager()
{
	Join();
}

auto ThreadManager::Launch(std::function<void(void)> callback) -> void
{
	LockGuard guard(_lock);


	_threads.push_back(std::thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

auto ThreadManager::Join() -> void
{
	for (std::thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

auto ThreadManager::InitTLS() -> void
{
	static Atomic<uint32> SThreadID = 1;
	LThreadId = SThreadID.fetch_add(1);
}

auto ThreadManager::DestroyTLS() -> void
{
}
