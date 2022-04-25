#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "SendBuffer.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"

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

	char sendData[1000] = "Hello World";

	while (true)
	{
		std::shared_ptr<SendBuffer>  sendBuffer = GSendBufferManager->Open(4096);

		BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());
		PacketHeader* header = bw.Reserve<PacketHeader>();

		// ID, 체력, 공격력
		bw << (uint64)1001 << (uint32)100 << (uint16)10;
		bw.Write(sendData, sizeof(sendData));

		header->size = bw.WriteSize();
		header->id = 1;

		sendBuffer->Close(bw.WriteSize());

		GGameSessionManager.BroadCast(sendBuffer);

		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}