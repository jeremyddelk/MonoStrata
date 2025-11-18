#include "pch.h"
#include "MonoStrataRuntime.h"
#include "MonoStrataRootDomain.h"
#include "Logger.h"

MonoStrataRootDomain* MonoStrataRootDomain::instance = nullptr;

extern "C" const char* SafeGetString(MonoRuntime* runtime, MonoImage* image, uint32_t index)
{
    const char* result = nullptr;

    __try {
        result = runtime->GetStringHeap(image, index);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        result = nullptr;
    }

    return result;
}

std::vector<std::string> MonoStrataRootDomain::GetMonoModule()
{
    std::vector<std::string> results;
    HMODULE hMods[1024];
    DWORD cbNeeded;
    HANDLE hProcess = GetCurrentProcess();

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); ++i)
        {
            char modName[MAX_PATH];
            if (GetModuleFileNameA(hMods[i], modName, MAX_PATH))
            {
                std::string name = modName;
                if (name.find("mono") != std::string::npos)
                {
                    HMODULE mod = hMods[i];
                    if (mod)
                    {
                        const char* symbols[] = {
                            "mono_get_root_domain",
                            "mono_assembly_get_image",
                            "mono_image_get_name",
                            "mono_image_get_table_info",
                            "mono_table_info_get_rows",
                            "mono_metadata_string_heap",
                            "mono_class_from_name",
                            "mono_class_get_methods",
                            "mono_method_get_name",
                            "mono_assembly_foreach",
                            "mono_image_get_filename"
                        };
                        int iter = 0;
                        int length = 0;
                        for (const char* sym : symbols)
                        {
                            length++;
                            FARPROC proc = GetProcAddress(mod, sym);
                            if (proc)
                                iter++;
                            else continue;
                        }

                        if (iter == length)
                        {
                            MessageBoxA(0, name.c_str(), name.c_str(), MB_OK);
                            results.emplace_back(name);
                        }
                    }
                }
            }
        }
    }
    
    return results;
}

MonoStrataRootDomain::MonoStrataRootDomain()
{
    instance = this;
    initialized = true;

    std::vector<std::string> modName = MonoStrataRootDomain::GetMonoModule();

    if (modName.empty()) {
        initialized = false;
        MessageBoxA(0, "No valid Mono runtimes found", "Initialization Error", MB_OK);
    }

    for (const auto& mod : modName)
    {
        LIB_HANDLE modHandle = LOAD_LIBRARY(mod.c_str());
        monoRuntimes.AddRuntime(mod, modHandle);

        if (!modHandle)
        {
            MessageBoxA(0, "Mono runtime initialization failed", mod.c_str(), MB_OK);
            initialized = false;
        }

    }
}

MonoStrataRootDomain* MonoStrataRootDomain::GetInstance()
{
    return instance;
}

MonoStrataRootDomain::~MonoStrataRootDomain()
{
    monoRuntimes.clear();

    if (instance)
    {
        instance = nullptr;
    }
}