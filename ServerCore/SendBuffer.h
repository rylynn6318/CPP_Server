#pragma once

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(std::shared_ptr<SendBufferChunk> owner, BYTE* buffer, int32 allocSize);
	~SendBuffer() = default;

	auto Buffer() -> BYTE*;
	auto WriteSize() -> int32;
	auto Close(uint32 writeSize) -> void;

private:
	std::shared_ptr<SendBufferChunk> _owner;
	BYTE* _buffer;
	uint32 _allocSize{ 0 };
	uint32 _writeSize{ 0 };
};

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	auto Reset() -> void;
	auto Open(uint32 allocSize) -> std::shared_ptr<SendBuffer>;
	auto Close(uint32 writeSize) -> void;

	auto IsOpen() -> bool;
	auto Buffer() -> BYTE*;
	auto FreeSize() -> uint32;

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer{};
	bool _open{ false };
	uint32 _usedSize{ 0 };
};

class SendBuffManager
{
public:
	auto Open(uint32 size) -> std::shared_ptr<SendBuffer>;
	
private:
	auto Push(std::shared_ptr<SendBufferChunk> buffer) -> void;
	auto Pop() -> std::shared_ptr<SendBufferChunk>;

	static auto PushGlobal(SendBufferChunk* buffer) -> void;


private:
	USE_LOCK;
	Vector<std::shared_ptr<SendBufferChunk>> _sendBufferChunk;
};
