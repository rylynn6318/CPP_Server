 #include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(butter);

	// TODO : 로그
	return false;
}

auto Handle_S_LOGIN(std::shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet) -> bool
{
	if (packet.success() == false)
		return true;

	if (packet.players().size() == 0)
	{
		// 캐릭터 생성창
	}

	// 입장 UI 버튼 눌러서 게임 입장
	Protocol::C_ENTER_GAME enterGamePacket;
	enterGamePacket.set_playerindex(0); // 첫번째 캐릭터로
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePacket);
	session->Send(sendBuffer);

	return true;
}

auto Handle_S_ENTER_GAME(std::shared_ptr<PacketSession>& session, Protocol::S_ENTER_GAME& packet) -> bool
{
	return true;
}

auto Handle_S_CHAT(std::shared_ptr<PacketSession>& session, Protocol::S_CHAT& packet) -> bool
{
	std::cout << packet.msg() << std::endl;
	return true;
}
