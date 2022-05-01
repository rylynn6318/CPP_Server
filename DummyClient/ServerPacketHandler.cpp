 #include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(butter);

	// TODO : ·Î±×
	return false;
}

auto Handle_S_TEST(std::shared_ptr<PacketSession>& session, Protocol::S_TEST& packet) -> bool
{
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

	return true;
}

auto Handle_S_LOGIN(std::shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet) -> bool
{
	return false;
}
