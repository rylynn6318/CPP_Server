#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

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

auto ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 length) -> void
{
	Protocol::S_TEST packet;

	ASSERT_CRASH(packet.ParseFromArray(buffer + sizeof(PacketHeader), length - sizeof(PacketHeader)));

	std::cout << packet.id() << " " << packet.hp() << " " << packet.attack() << std::endl;
	std::cout << "BUFSIZE: " << packet.buffs_size() << std::endl;

	for (auto& buf : packet.buffs())
	{
		std::cout << "BUFINFO: " << buf.buffid() << " " << buf.remaintime() << std::endl;
		std::cout << "VICTIMS: " << buf.victims_size() << std::endl;
		for (auto& vic : buf.victims())
		{
			std::cout << vic << " ";
		}

		std::cout << std::endl;
	}
}
