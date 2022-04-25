#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos) : _buffer(buffer), _size(size), _pos(pos)
{
}

BufferReader::~BufferReader()
{
}

auto BufferReader::Buffer() -> BYTE*
{
	return _buffer;
}

auto BufferReader::Size() -> uint32
{
	return _size;
}

auto BufferReader::ReadSize() -> uint32
{
	return _pos;
}

auto BufferReader::FreeSize() -> uint32
{
	return _size - _pos;
}

auto BufferReader::Peek(void* dest, uint32 length) -> bool
{
	if (FreeSize() < length) // 내가 읽어야 할 사이즈보다 작다면
		return false;

	::memcpy(dest, &_buffer[_pos], length);
	return true;
}

auto BufferReader::Read(void* dest, uint32 length) -> bool
{
	if (Peek(dest, length) == false)
		return false;

	_pos += length;
	return true;
}
