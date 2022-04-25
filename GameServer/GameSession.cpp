#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"

auto GameSession::OnConnected() -> void
{
	GGameSessionManager.Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

auto GameSession::OnDisconnected() -> void
{
	GGameSessionManager.Remove(std::static_pointer_cast<GameSession>(shared_from_this()));
}

auto GameSession::OnRecvPacket(BYTE* buffer, int32 len) -> void
{
	ServerPacketHandler::HandlePacket(buffer, len);
}

auto GameSession::OnSend(int32 len) -> void
{
}
