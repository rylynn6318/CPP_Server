#include "pch.h"
#include "Allocator.h"

auto BaseAllocator::Alloc(int32 size) -> void*
{
	return ::malloc(size);
}

auto BaseAllocator::Release(void* ptr) -> void*
{
	::free(ptr);
}
