#pragma once

class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();

	auto Buffer() -> BYTE*;
	auto Size() -> uint32;
	auto ReadSize() -> uint32;
	auto FreeSize() -> uint32;

	template<typename T>
	auto Peek(T* dest) -> bool
	{
		return Peek(dest, sizeof(T));
	}
	auto Peek(void* dest, uint32 length) -> bool;

	template<typename T>
	auto Read(T* dest) -> bool
	{
		return Read(dest, sizeof(T));
	}
	auto Read(void* dest, uint32 length) -> bool;

	template<typename T>
	auto operator>>(OUT T& dest) -> BufferReader&;

private:
	BYTE* _buffer{ nullptr };
	uint32 _size{ 0 };
	uint32 _pos{ 0 };
};

template<typename T>
inline auto BufferReader::operator>>(OUT T& dest) -> BufferReader&
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}
