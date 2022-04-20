#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

char SendBuffer[] = "Hello World!";

class ServerSession :public Session
{
public:
	~ServerSession()
	{
		std::cout << "~ServerSession" << std::endl;
	}

	virtual auto OnConnected() -> void override
	{
		std::cout << "Connected to Server" << std::endl;
		Send((BYTE*)SendBuffer, sizeof(SendBuffer));
	}

	virtual auto OnDisconnected() -> void override
	{
		std::cout << "Disconnected" << std::endl;
	}

	virtual auto OnRecv(BYTE* buffer, int32 len)->int32 override
	{
		std::cout << "OnRecv Len = " << len << std::endl;
		
		std::this_thread::sleep_for(1s);

		Send((BYTE*)SendBuffer, sizeof(SendBuffer));
		return len;
	}

	virtual auto OnSend(int32 len) ->void override
	{
		std::cout << "OnSend Len = " << len << std::endl;
	}
};

int main()
{
	std::this_thread::sleep_for(1s);

	std::shared_ptr<ClientService> service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1
		);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}