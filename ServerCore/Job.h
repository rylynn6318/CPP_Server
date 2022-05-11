#pragma once
#include <functional>

using CallBackType = std::function<void()>;

class Job
{
public:
	Job(CallBackType&& callback) : _callback(std::move(callback)) 
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	auto Execute() -> void;

private:
	CallBackType _callback;
};

