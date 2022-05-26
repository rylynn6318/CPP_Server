#pragma once

/*---------------
	ConsoleLog
----------------*/

enum class Color
{
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
};

class ConsoleLog
{
	enum { BUFFER_SIZE = 4096 };

public:
	ConsoleLog();
	~ConsoleLog();

public:
	auto WriteStdOut(Color color, const WCHAR* str, ...) -> void;
	auto WriteStdErr(Color color, const WCHAR* str, ...) -> void;

protected:
	auto SetColor(bool stdOut, Color color) -> void;

private:
	HANDLE	_stdOut;
	HANDLE	_stdErr;
};