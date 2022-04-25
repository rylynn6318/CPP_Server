#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"

auto ServerPacketHandler::HandlePacket(BYTE* buffer, int32 length) -> void
{
	BufferReader br(buffer, length);
	
	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	default:
		break;
	}
}

auto ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, std::vector<BuffData> buffs) -> std::shared_ptr<SendBuffer>
{
	std::shared_ptr<SendBuffer> sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());
	PacketHeader* header = bw.Reserve<PacketHeader>();

	// ID, 체력, 공격력
	bw << id << hp << attack;

	// 가변 데이터
	bw << (uint16)buffs.size();
	for (BuffData& buff : buffs)
	{
		bw << buff.buffId << buff.remainTime;
	}

	header->size = bw.WriteSize();
	header->id = S_TEST;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
