#pragma once
#include "DBConnection.h"

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	auto Connect(int32 connectionCount, const WCHAR* connectionString) -> bool;
	auto Clear() -> void;

	auto Pop() -> DBConnection*;
	auto Push(DBConnection* connection) -> void;

private:
	USE_LOCK;
	SQLHENV _environment{ SQL_NULL_HANDLE };
	Vector<DBConnection*> _connections;
};

