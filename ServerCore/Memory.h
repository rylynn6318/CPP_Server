#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum
	{
		// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	auto Allocate(int32 size) -> void*;
	auto Release(void* ptr) -> void;

private:
	std::vector<MemoryPool*> _pools;
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename ...Args>
auto xnew(Args&&... args) -> Type*
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
auto xdelete(Type* obj) -> void
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename ...Args>
 auto MakeShared(Args&&... args) -> std::shared_ptr<Type>
{
	return std::shared_ptr<Type>{xnew<Type>(std::forward<Args>(args)...), xdelete<Type>};
}