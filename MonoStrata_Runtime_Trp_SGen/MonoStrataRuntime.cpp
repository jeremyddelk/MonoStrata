#include "pch.h"
#include "MonoStrataRuntime.h"
#include "Logger.h"
#include "MonoPatchingService.h"
#include "ByteWalker.h"

MonoStrataRuntime::MonoStrataRuntime()
{
    PatchingService = nullptr;
    //Disassembler = nullptr
}

void MonoStrataRuntime::Start()
{
    PatchingService = new MonoPatchingService(this);
    //Disassembler = new ByteWalker(this);
}

void MonoStrataRuntime::Cleanup()
{
    if (PatchingService)
    {
        delete PatchingService;
        PatchingService = nullptr;
    }

    //if (Disassembler)
    //{
    //    delete Disassembler;
    //    Disassembler = nullptr;
    //}
}

MonoStrataRuntime::~MonoStrataRuntime()
{
    Cleanup();
}
