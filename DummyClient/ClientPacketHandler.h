#pragma once

enum
{
	S_TEST = 1
};

class ClientPacketHandler
{
public:
	static auto HandlePacket(BYTE* buffer, int32 length) -> void;

	static auto Handle_S_TEST(BYTE* buffer, int32 length) -> void;
};

