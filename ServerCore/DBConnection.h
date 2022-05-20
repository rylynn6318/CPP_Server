#pragma once
#include <sql.h>
#include <sqlext.h>

class DBConnection
{
public:
	auto Connect(SQLHENV henv, const WCHAR* connectionString) -> bool;
	auto Clear() -> void;

	auto Execute(const WCHAR* query) -> bool;
	auto Fetch() -> bool;
	auto GetRowCount() -> int32;
	auto Unbind() -> void;

public:
	auto BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN length, SQLPOINTER ptr, SQLLEN* index) -> bool;
	auto BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN length, SQLPOINTER value, SQLLEN* index) -> bool;
	auto HandleError(SQLRETURN ret) -> void;

private:
	SQLHDBC _connection{ SQL_NULL_HANDLE };
	SQLHSTMT _statement{ SQL_NULL_HANDLE };
};

