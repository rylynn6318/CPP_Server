#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer() = default;

	auto Clean() -> void;
	auto OnRead(int32 numOfBytes) -> bool;
	auto OnWrite(int32 numOfBytes) -> bool;

	auto ReadPos() -> BYTE*;
	auto WritePos() -> BYTE*;
	auto DataSize() -> int32;
	auto FreeSize() -> int32;

private:
	int32 _capacity{ 0 };
	int32 _bufferSize{ 0 };
	int32 _readPos{ 0 };
	int32 _writePos{ 0 };
	Vector<BYTE> _buffer;
};

