﻿#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

class GameSession :public Session
{
public:
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

	virtual auto OnRecv(BYTE* buffer, int32 len)->int32 override
	{
		std::cout << "OnRecv Len = " << len << std::endl;
		Send(buffer, len);
		return len;
	}

	virtual auto OnSend(int32 len) ->void override
	{
		std::cout << "OnSend Len = " << len << std::endl;
	}
};

int main()
{
	std::shared_ptr<ServerService> service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100
		);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
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