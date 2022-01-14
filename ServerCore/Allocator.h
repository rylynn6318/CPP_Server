#pragma once

class BaseAllocator
{
public:
	static auto Alloc(int32 size)->void*;
	static auto Release(void* ptr)->void*;
};

