#include "pch.h"

#include "Log.h"

namespace logging
{
	void Info(const char* format, ...)
	{
		static char _buffer[1024];

		va_list args;
		va_start(args, format);
		int written = _vsnprintf_s(_buffer, sizeof(_buffer) - 2, format, args);
		va_end(args);

		_buffer[written] = '\n';
		_buffer[written + 1] = '\0';

		OutputDebugStringA(_buffer);
	}
}