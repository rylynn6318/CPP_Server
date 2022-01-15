#pragma once

class BaseAllocator
{
public:
	static auto Alloc(int32 size)->void*;
	static auto Release(void* ptr)->void;
};

class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static auto Alloc(int32 size)->void*;
	static auto Release(void* ptr)->void;
};