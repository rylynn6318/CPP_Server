#pragma once

class Player;

class Room
{
public:
	auto Enter(std::shared_ptr<Player> player) -> void;
	auto Leave(std::shared_ptr<Player> player) -> void;
	auto BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void;

private:
	USE_LOCK;
	std::map <uint16, std::shared_ptr<Player>> _players;
};

extern Room GRoom;
