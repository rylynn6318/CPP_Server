#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_TEST = 1000,
	PKT_C_MOVE = 1001,
	PKT_S_TEST = 1002,
	PKT_S_LOGIN = 1003,
};

// Custom Handlers
auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool;
auto Handle_C_TEST(std::shared_ptr<PacketSession>& session, Protocol::C_TEST& packet) -> bool;
auto Handle_C_MOVE(std::shared_ptr<PacketSession>& session, Protocol::C_MOVE& packet) -> bool;

class ClientPacketHandler
{
public:
	static auto Init() -> void
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_TEST] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, session, butter, length);
		};
		GPacketHandler[PKT_C_MOVE] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, butter, length);
		};
	}

	static auto HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 length) -> bool
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, length);
	}
	static auto MakeSendBuffer(Protocol::S_TEST& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_S_TEST);
	}
	static auto MakeSendBuffer(Protocol::S_LOGIN& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_S_LOGIN);
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
