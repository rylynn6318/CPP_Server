#pragma once
#include "Session.h"

class GameSession :public PacketSession
{
public:
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

	virtual auto OnConnected() -> void override;
	virtual auto OnDisconnected() -> void override;
	virtual auto OnRecvPacket(BYTE* buffer, int32 len) -> void override;
	virtual auto OnSend(int32 len) ->void override;
};