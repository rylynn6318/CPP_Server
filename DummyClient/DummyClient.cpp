#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ClientPacketHandler.h"

char sendData[] = "Hello World!";

class ServerSession :public PacketSession
{
public:
	~ServerSession()
	{
		std::cout << "~ServerSession" << std::endl;
	}

	virtual auto OnConnected() -> void override
	{

	}

	virtual auto OnDisconnected() -> void override
	{
		std::cout << "Disconnected" << std::endl;
	}

	virtual auto OnRecvPacket(BYTE* buffer, int32 len) -> void override
	{
		ClientPacketHandler::HandlePacket(buffer, len);
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