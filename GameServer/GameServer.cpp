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

//CV는 User-Level Object
std::condition_variable cv;

void Producer()
{
	while (true)
	{
		//1) Lock을 잡고
		//2) 공유 변수 값을 수정
		//3) Lock을 풀고
		//4) 조건변수 통해 다른 쓰레드에게 통지
		{
			std::unique_lock<std::mutex> lock(m);
			q.push(100);
		}

		cv.notify_one(); //wait중인 쓰레드가 있으면 딱 1개를 깨운다.
	}
}

void Consumer()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });
		//1) Lock을 잡고
		//2) 조건 확인
		//	조건 만족 -> 빠져 나와서 코드를 진행
		//	조건 만족 X -> Lock을 풀어주고 대기 상태

		//그런데 notify_one을 했으면 항상 조건식을 만족하는거 아닐까?
		//Spurius Wakeup
		//notify_one을 할 때 lock을 잡고 있는 것이 아니기 때문

		{
			int32 data = q.front();
			q.pop();
			std::cout << q.size() << std::endl;
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
