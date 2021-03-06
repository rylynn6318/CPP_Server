#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

auto BaseAllocator::Alloc(int32 size) -> void*
{
	return ::malloc(size);
}

auto BaseAllocator::Release(void* ptr) -> void
{
	::free(ptr);
}

auto StompAllocator::Alloc(int32 size) -> void*
{
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

auto StompAllocator::Release(void* ptr) -> void
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RESERVE);
}

auto PoolAllocator::Alloc(int32 size) -> void*
{
	return GMemory->Allocate(size);
}

auto PoolAllocator::Release(void* ptr) -> void
{
	GMemory->Release(ptr);
}
