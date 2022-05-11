#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

auto JobTimer::Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job) -> void
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick, jobData });
}

auto JobTimer::Distribute(uint64 now) -> void
{
	// �� ���� �� �����常 ���
	if (_distributing.exchange(true) == true)
	{
		return;
	}

	Vector<TimerItem> items;
	{
		WRITE_LOCK;

		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.executeTick)
			{
				break;
			}

			items.push_back(timerItem);
			_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (std::shared_ptr<JobQueue> owner = item.jobData->owner.lock())
		{
			owner->Push(item.jobData->job, true);
		}

		ObjectPool<JobData>::Push(item.jobData);
	}

	// �������� Ǯ���ش�.
	_distributing.store(false);
}

auto JobTimer::Clear() -> void
{
	WRITE_LOCK;

	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		ObjectPool<JobData>::Push(timerItem.jobData);
		_items.pop();
	}
}
