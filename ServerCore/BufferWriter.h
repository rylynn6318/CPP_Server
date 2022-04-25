#pragma once

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	auto Buffer() -> BYTE*;
	auto Size() -> uint32;
	auto WriteSize() -> uint32;
	auto FreeSize() -> uint32;

	template<typename T>
	auto Write(T* source) -> bool
	{
		return Write(source, sizeof(T));
	}
	auto Write(void* source, uint32 length) -> bool;

	template<typename T>
	auto Reserve() -> T*;

	template<typename T>
	auto operator<<(T&& source) -> BufferWriter&;

private:
	BYTE* _buffer{ nullptr };
	uint32 _size{ 0 };
	uint32 _pos{ 0 };
};

template<typename T>
auto BufferWriter::Reserve() -> T*
{
	if (FreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ret;
}

template<typename T>
auto BufferWriter::operator<<(T&& source) -> BufferWriter&
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(source);
	_pos += sizeof(T);
	return *this;
}