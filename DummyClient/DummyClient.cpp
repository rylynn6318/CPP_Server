#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

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
		Protocol::C_LOGIN packet;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
		Send(sendBuffer);
	}

	virtual auto OnDisconnected() -> void override
	{
		std::cout << "Disconnected" << std::endl;
	}

	virtual auto OnRecvPacket(BYTE* buffer, int32 len) -> void override
	{
		std::shared_ptr<PacketSession> session = GetPacketSession();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual auto OnSend(int32 len) ->void override
	{
		std::cout << "OnSend Len = " << len << std::endl;
	}
};

int main()
{
	ServerPacketHandler::Init();

	std::this_thread::sleep_for(1s);

	std::shared_ptr<ClientService> service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		100
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

	Protocol::C_CHAT charPacket;
	charPacket.set_msg(u8"Hello World ! ");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(charPacket);

	while (true)
	{
		service->BroadCast(sendBuffer);
		std::this_thread::sleep_for(1s);
	}

	GThreadManager->Join();
}