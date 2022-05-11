#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

auto JobQueue::Push(std::shared_ptr<Job> job, bool pushOnly) -> void
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	// 첫번째 Job을 넣은 스레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 스레드가 실행
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

auto JobQueue::Execute() -> void
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<std::shared_ptr<Job>> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Execute();
		}

		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// 여유 있는 다른 스레드가 실행
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
