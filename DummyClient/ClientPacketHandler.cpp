#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

auto ClientPacketHandler::HandlePacket(BYTE* buffer, int32 length) -> void
{
	BufferReader br(buffer, length);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, length);
		break;
	default:
		break;
	}
}

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;
	std::vector<BuffData> buffs;
};

auto ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 length) -> void
{
	BufferReader br(buffer, length);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;

	std::cout << "ID : " << id << " HP: " << hp << " ATTACK:" << attack << std::endl;

	std::vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;
	buffs.resize(buffCount);
	for (int i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	std::cout << "BuffCount : " << buffCount << std::endl;

	for (BuffData& buff : buffs)
	{
		std::cout << "BuffInfo: " << buff.buffId << " " << buff.remainTime << std::endl;
	}
}
