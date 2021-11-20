#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <windows.h>

std::mutex m;
std::queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle);

		std::this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);

		std::unique_lock<std::mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			std::cout << data << endl;
		}
	}
}

int main()
{

	handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
