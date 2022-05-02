#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(butter);

	// TODO : �α�
	return false;
}

auto Handle_C_LOGIN(std::shared_ptr<PacketSession>& session, Protocol::C_LOGIN& packet) -> bool
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);

	Protocol::S_LOGIN LoginAck;
	LoginAck.set_success(true);

	static Atomic<uint64> idGenerator = 1;

	{
		auto player = LoginAck.add_players();
		player->set_name(u8"DB�����ܾ���̸�1");
		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

		std::shared_ptr<Player> playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}
	{
		auto player = LoginAck.add_players();
		player->set_name(u8"DB�����ܾ���̸�2");
		player->set_playertype(Protocol::PLAYER_TYPE_MAGE);

		std::shared_ptr<Player> playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(LoginAck);
	session->Send(sendBuffer);

	return true;
}

auto Handle_C_ENTER_GAME(std::shared_ptr<PacketSession>& session, Protocol::C_ENTER_GAME& packet) -> bool
{
	std::shared_ptr<GameSession> gameSession = std::static_pointer_cast<GameSession>(session);
	
	uint64 index = packet.playerindex();
	
	std::shared_ptr<Player> player = gameSession->_players[index];
	GRoom.Enter(player);

	Protocol::S_ENTER_GAME enterGameAck;
	enterGameAck.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGameAck);
	player->ownerSession->Send(sendBuffer);

	return true;
}

auto Handle_C_CHAT(std::shared_ptr<PacketSession>& session, Protocol::C_CHAT& packet) -> bool
{
	std::cout << packet.msg() << std::endl;

	Protocol::S_CHAT charPacket;
	charPacket.set_msg(packet.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(charPacket);

	GRoom.BroadCast(sendBuffer);

	return true;
}
