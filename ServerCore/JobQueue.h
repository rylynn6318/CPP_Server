#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	auto DoAsync(CallBackType&& callback) -> void
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	auto DoAsync(Ret(T::* memFunc)(Args...), Args... args) -> void
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
	}

	auto DoTimer(uint64 tickAfter, CallBackType&& callback) -> void
	{
		std::shared_ptr<Job> job = ObjectPool<Job>::MakeShared(std::move(callback));
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	auto DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args) -> void
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		std::shared_ptr<Job> job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);

	}

	auto ClearJob() -> void { _jobs.Clear(); }

public:
	auto Push(std::shared_ptr<Job> job, bool pushOnly = false) -> void;
	auto Execute() -> void;


protected:
	LockQueue<std::shared_ptr<Job>> _jobs;
	Atomic<int32> _jobCount{ 0 };
};

