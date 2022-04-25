#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter()
{
}

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 pos) : _buffer(buffer), _size(size), _pos(pos)
{
}

BufferWriter::~BufferWriter()
{
}

auto BufferWriter::Buffer() -> BYTE*
{
	return _buffer;
}

auto BufferWriter::Size() -> uint32
{
	return _size;
}

auto BufferWriter::WriteSize() -> uint32
{
	return _pos;
}

auto BufferWriter::FreeSize() -> uint32
{
	return _size - _pos;
}

auto BufferWriter::Write(void* source, uint32 length) -> bool
{
	if (FreeSize() < length)
		return false;

	::memcpy(&_buffer[_pos], source, length);
	_pos += length;
	return true;
}
