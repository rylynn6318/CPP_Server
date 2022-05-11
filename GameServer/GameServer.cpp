﻿#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "SendBuffer.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"

enum
{
	WORKER_TICK = 64
};

auto DoWorkerJob(std::shared_ptr<ServerService>& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() * WORKER_TICK;

		//네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	GRoom->DoTimer(1000, [] {std::cout << "Hello 1000 " << std::endl; });
	GRoom->DoTimer(2000, [] {std::cout << "Hello 2000 " << std::endl; });
	GRoom->DoTimer(3000, [] {std::cout << "Hello 3000 " << std::endl; });

	ClientPacketHandler::Init();

	std::shared_ptr<ServerService> service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100
		);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWorkerJob(service);
				}
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}