#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

std::shared_ptr<Room> GRoom = MakeShared<Room>();

auto Room::Enter(std::shared_ptr<Player> player) -> void
{
	_players[player->playerId] = player;
}

auto Room::Leave(std::shared_ptr<Player> player) -> void
{
	_players.erase(player->playerId);
}

auto Room::BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void
{
	for (auto& player : _players)
	{
		player.second->ownerSession->Send(sendBuffer);
	}
}
