#pragma once

#include "MonoStrataRuntime.h"

typedef void (*NativeLogDelegate)(const char* message);

class Logger
{
public:
	/// <summary>
	/// Attaches logger from unity
	/// </summary>
	/// <param name="logger"></param>
	static void AttachLogger(NativeLogDelegate logger);

	/// <summary>
	/// Log a message to unity console
	/// </summary>
	/// <param name="message"></param>
	/// <param name="">args</param>
	static void Log(const char* message, ...);

	/// <summary>
	/// Checks if LogToUnity delegate is null
	/// </summary>
	/// <returns></returns>
	static bool IsDelegateNull()
	{
		return LogToUnity == nullptr;
	}

	static void Destroy();

private:
	static NativeLogDelegate LogToUnity;
};