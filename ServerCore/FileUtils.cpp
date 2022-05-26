#include "pch.h"
#include "FileUtils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

auto FileUtils::ReadFile(const WCHAR* path) -> Vector<BYTE>
{
	Vector<BYTE> ret;

	fs::path filePath{ path };

	const uint32 fileSize = static_cast<uint32>(fs::file_size(filePath));
	ret.resize(fileSize);

	std::basic_ifstream<BYTE> inputStream{ filePath };
	inputStream.read(&ret[0], fileSize);

	return ret;
}

auto FileUtils::Convert(std::string str) -> String
{
	const int32 srcLen = static_cast<int32>(str.size());

	String ret;
	if (srcLen == 0)
		return ret;

	const int32 retLen = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, NULL, 0);
	ret.resize(retLen);
	::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, &ret[0], retLen);

	return ret;
}
