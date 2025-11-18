// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Logger.h"
#include "MonoStrataRootDomain.h"
#include "MonoPatchingService.h"
#include "MonoStrataRuntime.h"


MonoStrataRuntime Primary;

extern "C" __declspec(dllexport) void Initialize(NativeLogDelegate UnityLogger)
{
    Logger::AttachLogger(UnityLogger);

    Primary.Start();
}

extern "C" __declspec(dllexport) bool PatchMethod(unsigned long long targetHash, unsigned long long hookHash)
{
    return Primary.PatchingService->PatchMethod(targetHash, hookHash);;
}

extern "C" __declspec(dllexport) bool RestoreOriginalBytes(void* target, BYTE* originalBytes)
{
    return Primary.PatchingService->RestorePatchMethod(target, originalBytes);
}