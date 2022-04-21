#pragma once
#include "Session.h"

class GameSession :public Session
{
public:
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

	virtual auto OnConnected() -> void override;
	virtual auto OnDisconnected() -> void override;
	virtual auto OnRecv(BYTE* buffer, int32 len)->int32 override;
	virtual auto OnSend(int32 len) ->void override;
};