#include "pch.h"
#include "ClientPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(butter);

	// TODO : ·Î±×
	return false;
}

auto Handle_C_TEST(std::shared_ptr<PacketSession>& session, Protocol::C_TEST& packet) -> bool
{
	return false;
}

auto Handle_C_MOVE(std::shared_ptr<PacketSession>& session, Protocol::C_MOVE& packet) -> bool
{
	return false;
}

auto Handle_S_TEST(std::shared_ptr<PacketSession>& session, Protocol::S_TEST& packet) -> bool
{
	// TODO

	return false;
}

auto Handle_S_LOGIN(std::shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet) -> bool
{
	return false;
}
