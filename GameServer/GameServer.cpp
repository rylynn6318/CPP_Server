#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

int64 result;

int64 Calcuate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; i++)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<std::string>&& promise)
{
	promise.set_value("Secret Message"s);
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

int main()
{
	//동기 실행
	//int64 sum = Calcuate();
	//std::cout << sum << std::endl;

	//std::future
	{
		//1) deferred -> lazy evaluation 지연해서 실행하세요
		//2) async -> 별도의 쓰레드를 만들어서 실행하세요
		//3) deferred | async -> 둘 중 알아서 골라주세요
		std::future<int64> future = std::async(std::launch::async, Calcuate);

		//TODO

		int64 sum = future.get();
	}

	//std::promise
	{
		//미래에 결과물을 반환해줄거라 약속
		std::promise<std::string> promise;
		std::future<std::string> future = promise.get_future();

		std::thread t(PromiseWorker, std::move(promise));

		std::string message = future.get();
		std::cout << message << std::endl;

		t.join();
	}

	//std::packged_task
	{
		std::packaged_task<int64(void)> task(Calcuate);
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));

		int64 sum = future.get();
		std::cout << sum << std::endl;

		t.join();
	}
}
