#pragma once

class Job;

template<typename T>
class LockQueue
{
public:
	auto Push(T item) -> void
	{
		WRITE_LOCK;
		_items.push(item);
	}

	auto Pop() -> T
	{
		WRITE_LOCK;
		if (_items.empty())
			return T();

		T ret = _items.front();
		_items.pop();
		return ret;
	}
	
	auto PopAll(OUT Vector<T>& items) -> void
	{
		WRITE_LOCK;
		while (T item = Pop())
		{
			items.push_back(item);
		}
	}

	auto Clear() -> void
	{
		WRITE_LOCK;
		_items = Queue<T>();
	}

private:
	USE_LOCK;
	Queue<T> _items;
};
