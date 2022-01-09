#pragma once

#include <thread>
#include <functional>


class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	auto Launch(std::function<void(void)> callback) -> void;
	auto Join() -> void;

	static auto InitTLS() -> void;
	static auto DestroyTLS() -> void;

private:
	Mutex _lock;
	std::vector<thread> _threads;
};
