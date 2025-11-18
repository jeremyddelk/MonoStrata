#include "pch.h"
#include "Logger.h"

NativeLogDelegate Logger::LogToUnity = nullptr;

void Logger::AttachLogger(NativeLogDelegate logger)
{
	LogToUnity = logger;
}

void Logger::Log(const char* message, ...)
{
	if (LogToUnity)
	{
		char _buffer[512];

		va_list args;
		va_start(args, message);
		vsnprintf(_buffer, sizeof(_buffer), message, args);
		va_end(args);

		LogToUnity(_buffer);
	}
}

void Logger::Destroy()
{
	LogToUnity = nullptr;
}
