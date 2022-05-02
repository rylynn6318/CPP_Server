#pragma once

class GameSession;

class Player
{
public:

	uint16 playerId = 0;
	std::string name;
	Protocol::PlayerType type = Protocol::PLAYER_TYPE_NONE;
	std::shared_ptr<GameSession> ownerSession;
};

