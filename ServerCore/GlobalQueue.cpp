#include "pch.h"
#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

auto GlobalQueue::Push(std::shared_ptr<JobQueue> jobQueue) -> void
{
	_jobQueues.Push(jobQueue);
}

auto GlobalQueue::Pop() -> std::shared_ptr<JobQueue>
{
	return _jobQueues.Pop();
}
