#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
};

// Custom Handlers
auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool;
auto Handle_S_LOGIN(std::shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet) -> bool;
auto Handle_S_ENTER_GAME(std::shared_ptr<PacketSession>& session, Protocol::S_ENTER_GAME& packet) -> bool;
auto Handle_S_CHAT(std::shared_ptr<PacketSession>& session, Protocol::S_CHAT& packet) -> bool;

class ServerPacketHandler
{
public:
	static auto Init() -> void
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, butter, length);
		};
		GPacketHandler[PKT_S_ENTER_GAME] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, butter, length);
		};
		GPacketHandler[PKT_S_CHAT] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, butter, length);
		};
	}

	static auto HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 length) -> bool
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, length);
	}
	static auto MakeSendBuffer(Protocol::C_LOGIN& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_C_LOGIN);
	}
	static auto MakeSendBuffer(Protocol::C_ENTER_GAME& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_C_ENTER_GAME);
	}
	static auto MakeSendBuffer(Protocol::C_CHAT& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_C_CHAT);
	}


private:
	template<typename PacketType, typename ProcessFunc>
	static auto HandlePacket(ProcessFunc func, std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 length) -> bool
	{
		PacketType packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), length - sizeof(PacketHeader)) == false)
			return false;

		return func(session, packet);
	}

	template<typename T>
	static auto MakeSendBuffer(T& packet, uint16 packetId) -> std::shared_ptr<SendBuffer>
	{
		const uint16 dataSize = static_cast<uint16>(packet.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		std::shared_ptr<SendBuffer> sendBuffer = GSendBufferManager->Open(packetSize);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = packetId;
		ASSERT_CRASH(packet.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
