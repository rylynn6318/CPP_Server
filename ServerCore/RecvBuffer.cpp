#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

auto RecvBuffer::Clean() -> void
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_readPos = 0;
		_writePos = 0;
	}
	else
	{
		::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
		_readPos = 0;
		_writePos = dataSize;
	}
}

auto RecvBuffer::OnRead(int32 numOfBytes) -> bool
{
	if (DataSize() < numOfBytes)
		return false;

	_readPos += numOfBytes;
	return true;
}

auto RecvBuffer::OnWrite(int32 numOfBytes) -> bool
{
	if (FreeSize() < numOfBytes)
		return false;

	_writePos += numOfBytes;
	return true;
}

auto RecvBuffer::ReadPos() -> BYTE*
{
	return &_buffer[_readPos];
}

auto RecvBuffer::WritePos() -> BYTE*
{
	return &_buffer[_writePos];
}

auto RecvBuffer::DataSize() -> int32
{
	return _writePos - _readPos;
}

auto RecvBuffer::FreeSize() -> int32
{
	return _capacity - _writePos;
}
