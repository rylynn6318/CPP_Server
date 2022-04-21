#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 bufferSize)
{
	_buffer.resize(bufferSize);
}

auto SendBuffer::Buffer() -> BYTE*
{
	return _buffer.data();
}

auto SendBuffer::WriteSize() -> int32
{
	return _writeSize;
}

auto SendBuffer::Capacity() -> int32
{
	return static_cast<int32>(_buffer.size());
}

auto SendBuffer::CopyData(void* data, int32 length) -> void
{
	ASSERT_CRASH(Capacity() >= length);
	::memcpy(_buffer.data(), data, length);
	_writeSize = length;
}
