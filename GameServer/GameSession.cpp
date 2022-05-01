#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"

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
	std::shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packet ID 대역 체크

	ClientPacketHandler::HandlePacket(session, buffer, len);
}

auto GameSession::OnSend(int32 len) -> void
{
}
