#pragma once

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

// TODO: Multiplatform me
#include <windows.h>
#include <psapi.h>
#include <string>
#include <stdexcept>
#include <vadefs.h>
#include <unordered_set>
#include <cstdint>
#include <vector>
#include <memory>
#include "framework.h"

// TODO: Multiplatform me
#define GET_PROC(lib, name) GetProcAddress(lib, name)
#define LOAD_LIBRARY(path) LoadLibraryA(path)
#define LIB_HANDLE HMODULE

#define BIND_MONO_FUNC(lib, funcName, alias) alias = (decltype(alias))GET_PROC(lib, #funcName)
