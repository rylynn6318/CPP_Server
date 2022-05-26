#pragma once
#include <vector>
#include "Types.h"

class FileUtils
{
public:
	static auto	ReadFile(const WCHAR* path) -> Vector<BYTE>;
	static auto	Convert(std::string str) -> String;
};