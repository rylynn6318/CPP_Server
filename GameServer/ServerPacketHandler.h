#pragma once
#include "Protocol.pb.h"

enum
{
	S_TEST = 1
};

class ServerPacketHandler
{
public:
	static auto HandlePacket(BYTE* buffer, int32 length) -> void;

	static auto MakeSendBuffer(Protocol::S_TEST& packet) -> std::shared_ptr<SendBuffer>;
};

template<typename T>
auto _MakeSendBuffer(T& packet, uint16 packetId) -> std::shared_ptr<SendBuffer>
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

