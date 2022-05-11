#pragma once

class JobQueue;

struct JobData
{
	JobData(std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job) : owner(owner), job(job)
	{
	}

	std::weak_ptr<JobQueue> owner;
	std::shared_ptr<Job> job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	uint64 executeTick{ 0 };
	JobData* jobData{ nullptr };
};

class JobTimer
{
public:
	auto Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job) -> void;
	auto Distribute(uint64 now) -> void;
	auto Clear() -> void;

private:
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing{ false };
};

