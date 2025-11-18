#include "pch.h"
#include "MonoStrataRuntime.h"
#include "Logger.h"
#include "ByteWalker.h"

ByteWalker::ByteWalker(MonoStrataRuntime* Primary)
{
	this->Primary = Primary;
}

std::string ByteWalker::GetDisassembly(void* method)
{
	return std::string();
}

size_t ByteWalker::GetMethodSize(void* method)
{
	return size_t();
}

size_t ByteWalker::GetRoughMethodSize(void* method)
{
    BYTE* addr = (BYTE*)method;
    size_t totalSize = 0;

    MEMORY_BASIC_INFORMATION mbi;

    while (addr < sysInfo->MaximumApplicationAddress)
    {
        if (VirtualQuery(addr, &mbi, sizeof(mbi) == sizeof(mbi) && ((mbi.Protect & PAGE_EXECUTE_READ) || mbi.Protect & PAGE_EXECUTE_READWRITE)))
        {
            Opcode opcode = Disassemble(addr);


            addr += mbi.RegionSize;
        }
    }
	return size_t(); 
}

Opcode ByteWalker::Disassemble(BYTE* code)
{
    BYTE* start = code;
    Opcode opcodeInstruction;
    opcodeInstruction.size = 0;
    opcodeInstruction.mnemonic = "unknown";

	while (*code == 0x66 || *code == 0x67 || (*code >= 0x40 && *code <= 0x4F)) {
		opcodeInstruction.size++;
		code++;
	}

	uint8_t opcode = *code++;
	opcodeInstruction.size++;

	for (const auto& group : opcodes) {
		for (const auto& op : group) {
			if (op.hexCode == opcode) {
				opcodeInstruction.mnemonic = op.mnemonic;
				opcodeInstruction.size = op.size;
				opcodeInstruction.hexCode = opcode;
				return opcodeInstruction;
			}
		}
	}

    return opcodeInstruction;
}


void* ByteWalker::GetMaximumApplicationAddress()
{
	SetSystemInfo();
	return sysInfo->MaximumApplicationAddress;
}

void* ByteWalker::GetMinimumApplicationAddress()
{
	SetSystemInfo();
	return sysInfo->MinimumApplicationAddress;
}

void ByteWalker::SetSystemInfo()
{
	if (this->sysInfo == nullptr)
	{
		sysInfo = new SystemInformation();
		SYSTEM_INFO thisSys;
		GetSystemInfo(&thisSys);

		sysInfo->MaximumApplicationAddress = thisSys.lpMaximumApplicationAddress;
		sysInfo->MinimumApplicationAddress = thisSys.lpMinimumApplicationAddress;

		sysInfo->NumOfProcessors = thisSys.dwNumberOfProcessors;
	}
}

ByteWalker::~ByteWalker()
{
	if (sysInfo)
	{
		delete sysInfo;
		sysInfo = nullptr;
	}
}
