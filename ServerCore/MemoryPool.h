#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryHeader : public SLIST_ENTRY
{
public:
	MemoryHeader(int32 size) : allocSize(size) {}

	static auto AttachHeader(MemoryHeader* header, int32 size) -> void*
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	static auto DetachHeader(void* ptr) -> MemoryHeader*
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	auto Push(MemoryHeader* ptr)->void;
	auto Pop()->MemoryHeader*;

private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	std::atomic<int32> _useCount = 0;
	std::atomic<int32> _reserveCount = 0;
};

