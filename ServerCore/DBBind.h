#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits
{
	enum { value = (1 << (C - 1)) | FullBits<C - 1>::value };
};

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };


template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query) : _dbConnection(dbConnection), _query(query)
	{
		::memset(_paramIndex, 0, sizeof(_paramIndex));
		::memset(_columnIndex, 0, sizeof(_columnIndex));
		_paramFlag = 0;
		_columnFlag = 0;
		dbConnection.Unbind();
	}

	auto Validate() -> bool
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnCount>::value;
	}

	auto Execute() -> bool
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query);
	}

	auto Fetch() -> bool
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	auto BindParam(int32 idx, T& value) -> void
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	auto BindParam(int32 idx, const WCHAR* value) -> void
	{
		_dbConnection.BindParam(idx + 1, value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	auto BindParam(int32 idx, T(&value)[N]) -> void
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	auto BindParam(int32 idx, T* value, int32 N) -> void
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	auto BindCol(int32 idx, T& value) -> void
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<int32 N>
	auto BindCol(int32 idx, WCHAR(&value)[N]) -> void
	{
		_dbConnection.BindCol(idx + 1, value, N - 1, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	auto BindCol(int32 idx, WCHAR* value, int32 length) -> void
	{
		_dbConnection.BindCol(idx + 1, value, length - 1, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	auto BindCol(int32 idx, T(&value)[N]) -> void
	{
		_dbConnection.BindCol(idx + 1, value, size32(T) * N, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN _paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN _columnIndex[ColumnCount > 0 ? ColumnCount : 1];
	uint64 _paramFlag;
	uint64 _columnFlag;
};

