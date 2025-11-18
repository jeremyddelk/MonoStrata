#include "pch.h"
#include "Hash.h"

void* Hash::UnWravelHash(unsigned long long hash)
{
	return reinterpret_cast<void*>(hash ^ 0xDEADBEEFCAFEBABE);
}
