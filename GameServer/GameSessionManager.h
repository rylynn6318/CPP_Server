#pragma once

class GameSession;

class GameSessionManager
{
public:
	auto Add(std::shared_ptr<GameSession> session) -> void;
	auto Remove(std::shared_ptr<GameSession> session) -> void;
	auto BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void;

private:
	USE_LOCK;
	Set<std::shared_ptr<GameSession>> _sessions;
};

extern GameSessionManager GGameSessionManager;

