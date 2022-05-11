#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"
#include "JobQueue.h"

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

auto ThreadManager::DoGlobalQueueWork() -> void
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount)
		{
			break;
		}

		std::shared_ptr<JobQueue> jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
		{
			break;
		}

		jobQueue->Execute();
	}
}

auto ThreadManager::DistributeReservedJobs() -> void
{
	const uint64 now = ::GetTickCount64();

	GJobTimer->Distribute(now);
}
