#include "pch.h"
#include "MonoPatchingService.h"
#include "MonoStrataRuntime.h"
#include "Logger.h"

MonoPatchingService::MonoPatchingService(MonoStrataRuntime* Primary)
{
    this->Primary = Primary;
}

bool MonoPatchingService::PatchMethod(unsigned long long targetHash, unsigned long long hookHash)
{

    if (!targetHash || !hookHash) {
        Logger::Log("[MonoStrata] Invalid patch parameters.");
        return false;
    }

    void* target = reinterpret_cast<void*>(targetHash ^ 0xD35EBEEF5576UL);
    void* hook = reinterpret_cast<void*>(hookHash ^ 0xD35EBEEF5576UL);


    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(target, &mbi, sizeof(mbi)) != sizeof(mbi) ||
        !(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)))
    {
        Logger::Log("[MonoStrata] Unwrapped method pointer is not executable.");
        return false;
    }

    if (VirtualQuery(hook, &mbi, sizeof(mbi)) != sizeof(mbi) ||
        !(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)))
    {
        Logger::Log("[MonoStrata] Unwrapped method pointer is not executable.");
        return false;
    }
        

    Logger::Log("[MonoStrata] Patching method at address: %p, hook: %p", target, hook);

    // Assemble the jump instruction (JMP RAX)
    BYTE patch[14] = { 0x48, 0xB8 }; // mov rax, [hook address]
    *reinterpret_cast<void**>(&patch[2]) = hook;
    patch[10] = 0xFF;
    patch[11] = 0xE0; // jmp rax

    DWORD oldProtect;
    if (VirtualProtect(target, sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        memcpy(target, patch, sizeof(patch));
        if (!VirtualProtect(target, 14, oldProtect, &oldProtect))
        {
            return false;
            Logger::Log("[MonoStrata] VirtualProtect failed.");
        }
        Logger::Log("[MonoStrata] Method patched successfully.");
        return true;
    }

    Logger::Log("[MonoStrata] VirtualProtect failed.");
    return false;
}

bool MonoPatchingService::RestorePatchMethod(void* target, BYTE* originalBytes)
{
    DWORD oldProtect;
    if (VirtualProtect(target, 14, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memcpy(target, originalBytes, 14);
        if (!VirtualProtect(target, 14, oldProtect, &oldProtect))
        {
            return false;
            Logger::Log("[MonoStrata] VirtualProtect failed.\n");
        }
        Logger::Log("[MonoStrata] Method restoration successful.\n");
        return true;
    }

    Logger::Log("[MonoStrata] VirtualProtect failed.\n");
    return false;
}

MonoPatchingService::~MonoPatchingService()
{
    if (Primary)
    {
        Primary = nullptr;
    }
}
