﻿#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <thread>
#include <mutex>

class SpinLock
{
public:
	void lock()
	{
		bool expected = false;
		bool desired = true;

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			std::this_thread::sleep_for(100ms);
		}

	}

	void unlock()
	{
		_locked.store(false);
	}
private:
	std::atomic<bool> _locked = false;
};

int32 sum = 0;
std::mutex m;
SpinLock spinLock;

void Add()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		std::lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		std::lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}

int main()
{
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << sum << std::endl;
}
