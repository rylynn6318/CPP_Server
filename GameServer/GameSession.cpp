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

auto GameSession::OnRecvPacket(BYTE* buffer, int32 len)->int32
{
	PacketHeader header = *(reinterpret_cast<PacketHeader*>(& buffer[0]));
	std::cout << "Packet ID: " << header.id << " Size: " << header.size << std::endl;

	return len;
}

auto GameSession::OnSend(int32 len) ->void
{
}
