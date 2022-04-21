#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

auto GameSession::OnConnected() -> void
{
	GGameSessionManager.Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

auto GameSession::OnDisconnected() -> void
{
	GGameSessionManager.Remove(std::static_pointer_cast<GameSession>(shared_from_this()));
}

auto GameSession::OnRecv(BYTE* buffer, int32 len)->int32
{
	std::cout << "OnRecv Len = " << len << std::endl;

	std::shared_ptr<SendBuffer> sendBuffer = MakeShared<SendBuffer>(4096);
	sendBuffer->CopyData(buffer, len);
	GGameSessionManager.BroadCast(sendBuffer);

	return len;
}

auto GameSession::OnSend(int32 len) ->void
{
	std::cout << "OnSend Len = " << len << std::endl;
}