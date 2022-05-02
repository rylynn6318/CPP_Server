#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

Room GRoom;

auto Room::Enter(std::shared_ptr<Player> player) -> void
{
	WRITE_LOCK;
	_players[player->playerId] = player;
}

auto Room::Leave(std::shared_ptr<Player> player) -> void
{
	WRITE_LOCK;
	_players.erase(player->playerId);
}

auto Room::BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void
{
	WRITE_LOCK;
	for (auto& player : _players)
	{
		player.second->ownerSession->Send(sendBuffer);
	}
}
