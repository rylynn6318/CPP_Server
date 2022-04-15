#pragma once
#include <stack>
#include <map>
#include <vector>

//---------------------------
//		DeadLockProfiler
//---------------------------
class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 here);

private:
	std::unordered_map<const char*, int32> _nameToId;
	std::unordered_map<int32, const char*> _idToName;
	std::stack<int32> _lockStack;
	std::map<int32, std::set<int32>> _lockHistory;

	Mutex _lock;

private:
	std::vector<int32> _discoveredOrder;	//�߰� ���� ����ϴ� �迭
	int32 _discoveredCount = 0;	//��尡 �߰ߵ� ����
	std::vector<bool> _finished;	//Dfs(i)�� ���� �Ǿ����� ����
	std::vector<int32> _parent;
};
