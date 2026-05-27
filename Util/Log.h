#pragma once
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
using namespace std;
namespace base{
class Log{
	enum Level{
		__TRACE,
			__DEBUG,
			__INFO,
			__WARN,
			__ERROR,
			__FATAL,
			__SHOW_ERROR
	};
	static ofstream* _out;
public:
	static BOOL Open( const char* file_name );
	static BOOL Close();
	static void Out(Level level, HWND hWnd, va_list pArgs, const char* szFormat,  ... );
	static void Debug( const char* szFormat,  ... );
	static void Warn( const char* szFormat, ... );
	static void Error( const char* szFormat,  ... );
	static void Fatal( const char* szFormat, ... );
	static void ShowError( HWND hWnd, const char* szFormat, ... );
};
}