#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager GGameSessionManager;

auto GameSessionManager::Add(std::shared_ptr<GameSession> session) -> void
{
	WRITE_LOCK;
	_sessions.insert(session);
}

auto GameSessionManager::Remove(std::shared_ptr<GameSession> session) -> void
{
	WRITE_LOCK;
	_sessions.erase(session);
}

auto GameSessionManager::BroadCast(std::shared_ptr<SendBuffer> sendBuffer) -> void
{
	WRITE_LOCK;
	for (std::shared_ptr<GameSession> session : _sessions)
	{
		session->Send(sendBuffer);
	}
}
