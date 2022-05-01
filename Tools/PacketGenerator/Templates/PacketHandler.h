#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
{%- for packet in parser.total_pkt %}
	PKT_{{packet.name}} = {{packet.id}},
{%- endfor %}
};

// Custom Handlers
auto Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) -> bool;

{%- for packet in parser.recv_pkt %}
auto Handle_{{packet.name}}(std::shared_ptr<PacketSession>& session, Protocol::{{packet.name}}& packet) -> bool;
{%- endfor %}

class {{output}}
{
public:
	static auto Init() -> void
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

{%- for packet in parser.recv_pkt %}
		GPacketHandler[PKT_{{packet.name}}] = [](std::shared_ptr<PacketSession>& session, BYTE* butter, int32 length) {
			return HandlePacket<Protocol::{{packet.name}}>(Handle_{{packet.name}}, session, butter, length);
		};
{%- endfor %}
	}

	static auto HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 length) -> bool
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, length);
	}


{%- for packet in parser.send_pkt %}
	static auto MakeSendBuffer(Protocol::{{packet.name}}& packet) -> std::shared_ptr<SendBuffer>
	{
		return MakeSendBuffer(packet, PKT_{{packet.name}});
	}

{%- endfor %}


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

