#pragma once

class SendBuffer : std::enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer() = default;

	auto Buffer() -> BYTE*;
	auto WriteSize() -> int32;
	auto Capacity() -> int32;

	auto CopyData(void* data, int32 length) -> void;

private:
	Vector<BYTE> _buffer;
	int32 _writeSize{ 0 };
};

