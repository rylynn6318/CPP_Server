#include "pch.h"
#include "DBConnection.h"

auto DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString) -> bool
{
	if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
		return false;

	WCHAR stringBuffer[MAX_PATH] = { 0 };
	::wcscpy_s(stringBuffer, connectionString);

	WCHAR resultString[MAX_PATH] = { 0 };
	SQLSMALLINT resultStringLen = 0;

	SQLRETURN ret = ::SQLDriverConnectW(
		_connection,
		NULL,
		reinterpret_cast<SQLWCHAR*>(stringBuffer),
		_countof(stringBuffer),
		OUT reinterpret_cast<SQLWCHAR*>(resultString),
		_countof(resultString),
		OUT & resultStringLen,
		SQL_DRIVER_NOPROMPT
	);

	if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
		return false;

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

auto DBConnection::Clear() -> void
{
	if (_connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, _connection);
		_connection = SQL_NULL_HANDLE;
	}

	if (_connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, _statement);
		_statement = SQL_NULL_HANDLE;
	}
}

auto DBConnection::Execute(const WCHAR* query) -> bool
{
	SQLRETURN ret = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return true;

	HandleError(ret);
	return false;
}

auto DBConnection::Fetch() -> bool
{
	SQLRETURN ret = ::SQLFetch(_statement);

	switch (ret)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_NO_DATA:
		return false;
	case SQL_ERROR:
		HandleError(ret);
		return false;
	default:
		return true;
	}
}

auto DBConnection::GetRowCount() -> int32
{
	SQLLEN count = 0;
	SQLRETURN ret = ::SQLRowCount(_statement, OUT & count);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return static_cast<int32>(count);

	return -1;
}

auto DBConnection::Unbind() -> void
{
	::SQLFreeStmt(_statement, SQL_UNBIND);
	::SQLFreeStmt(_statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(_statement, SQL_CLOSE);
}

auto DBConnection::BindParam(int32 paramIndex, bool* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(bool), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, float* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_FLOAT, SQL_REAL, 0, value, index);
}

auto DBConnection::BindParam(int32 paramIndex, double* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_DOUBLE, SQL_DOUBLE, 0, value, index);
}

auto DBConnection::BindParam(int32 paramIndex, int8* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(int8), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, int16* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_SHORT, SQL_SMALLINT, size32(int16), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, int32* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_LONG, SQL_INTEGER, size32(int32), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, int64* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_SBIGINT, SQL_BIGINT, size32(int64), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool
{
	return BindParam(paramIndex, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

auto DBConnection::BindParam(int32 paramIndex, const WCHAR* str, SQLLEN* index) -> bool
{
	SQLULEN size = static_cast<SQLULEN>((::wcslen(str) + 1) * 2);
	*index = SQL_NTSL;

	if (size > WVARCHAR_MAX)
		return BindParam(paramIndex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)str, index);
	else
		return BindParam(paramIndex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)str, index);
}

auto DBConnection::BindParam(int32 paramIndex, const BYTE* bin, int32 size, SQLLEN* index) -> bool
{
	if (bin == nullptr)
	{
		*index = SQL_NULL_DATA;
		size = 1;
	}
	else
		*index = size;

	if (size > BINARY_MAX)
		return BindParam(paramIndex, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)bin, index);
	else
		return BindParam(paramIndex, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)bin, index);
}

auto DBConnection::BindCol(int32 columnIndex, bool* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_TINYINT, size32(bool), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, float* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_FLOAT, size32(float), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, double* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_DOUBLE, size32(double), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, int8* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_TINYINT, size32(int8), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, int16* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_SHORT, size32(int16), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, int32* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_LONG, size32(int32), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, int64* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_SBIGINT, size32(int64), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_TYPE_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

auto DBConnection::BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_C_WCHAR, size, str, index);
}

auto DBConnection::BindCol(int32 columnIndex, BYTE* bin, int32 size, SQLLEN* index) -> bool
{
	return BindCol(columnIndex, SQL_BINARY, size, bin, index);
}

auto DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN length, SQLPOINTER ptr, SQLLEN* index) -> bool
{
	SQLRETURN ret = ::SQLBindParameter(_statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, length, 0, ptr, 0, index);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

auto DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN length, SQLPOINTER value, SQLLEN* index) -> bool
{
	SQLRETURN ret = ::SQLBindCol(_statement, columnIndex, cType, value, length, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

auto DBConnection::HandleError(SQLRETURN ret) -> void
{
	if (ret == SQL_SUCCESS)
		return;

	SQLSMALLINT index = 1;
	SQLWCHAR sqlState[MAX_PATH] = { 0 };
	SQLINTEGER nativeError = 0;
	SQLWCHAR errorMsg[MAX_PATH] = { 0 };
	SQLSMALLINT msgLength = 0;
	SQLRETURN errorReturn = 0;

	while (true)
	{
		errorReturn = ::SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			_statement,
			index,
			sqlState,
			OUT & nativeError,
			errorMsg,
			_countof(errorMsg),
			OUT & msgLength
		);

		if (errorReturn == SQL_NO_DATA)
			break;

		if (errorReturn != SQL_SUCCESS && errorReturn != SQL_SUCCESS_WITH_INFO)
			break;

		std::wcout.imbue(std::locale("kor"));
		std::wcout << errorMsg << std::endl;

		index++;
	}
}
