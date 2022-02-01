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


class PoolAllocator
{
public:
	static auto Alloc(int32 size)->void*;
	static auto Release(void* ptr)->void;
};


template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() = default;

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};
