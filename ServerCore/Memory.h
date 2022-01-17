#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum
	{
		// ~1024���� 32����, ~2048���� 128����, ~4096���� 256����
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	auto Allocate(int32 size)->void*;
	auto Release(void* ptr)->void;

private:
	vector<MemoryPool*> _pools;
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename ...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	xrelease(obj);
}