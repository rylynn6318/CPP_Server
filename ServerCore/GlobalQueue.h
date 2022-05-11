#pragma once

class JobQueue;

class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	auto Push(std::shared_ptr<JobQueue> jobQueue) -> void;
	auto Pop() -> std::shared_ptr<JobQueue>;

private:
	LockQueue<std::shared_ptr<JobQueue>> _jobQueues;
};

