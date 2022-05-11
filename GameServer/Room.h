#pragma once
#include "JobQueue.h"

class Player;

class Room : public JobQueue
{
public:
	auto Enter(std::shared_ptr<Player> player) -> void;
	auto Leave(std::shared_ptr<Player> player) -> void;
	auto BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void;

private:
	std::map <uint16, std::shared_ptr<Player>> _players;
};

extern std::shared_ptr<Room> GRoom;
