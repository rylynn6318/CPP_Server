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

auto ServerPacketHandler::MakeSendBuffer(Protocol::S_TEST& packet) -> std::shared_ptr<SendBuffer>
{
	return _MakeSendBuffer(packet, S_TEST);
}

