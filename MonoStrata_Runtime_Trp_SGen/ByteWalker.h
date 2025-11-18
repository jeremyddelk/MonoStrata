#pragma once

class MonoStrataRuntime;

#define MAX_OPCODE_SIZE 0xF

struct Opcode{
	std::string mnemonic;
	int hexCode;
	size_t size;

	Opcode(const std::string& m, int h, size_t s) : mnemonic(m), hexCode(h), size(s) {}
	Opcode() {};
};

static std::vector<std::vector<Opcode>> opcodes = {
	// Arithmetic
	{
		{"add", 0x01, 2}, {"or", 0x09, 2}, {"adc", 0x11, 2}, {"sbb", 0x19, 2},
		{"sub", 0x29, 2}, {"xor", 0x31, 2}, {"cmp", 0x39, 2}, {"inc", 0x40, 1},
		{"dec", 0x48, 1}, {"neg", 0xF7, 2}, {"mul", 0xF6, 2}, {"imul", 0x69, 3},
		{"div", 0xF7, 2}, {"idiv", 0xF7, 2}
	},
	// Jump
	{
		{"jo", 0x70, 2}, {"jno", 0x71, 2}, {"jb", 0x72, 2}, {"jnb", 0x73, 2},
		{"je", 0x74, 2}, {"jne", 0x75, 2}, {"jl", 0x7C, 2}, {"jge", 0x7D, 2},
		{"jp", 0x7A, 2}, {"jnp", 0x7B, 2}, {"jmp", 0xE9, 5}, {"jmp short", 0xEB, 2},
		{"loop", 0xE2, 2}, {"loopz", 0xE1, 2}, {"loopnz", 0xE0, 2}
	},
	// Control Flow
	{
		{"call", 0xE8, 5}, {"ret", 0xC3, 1}, {"int", 0xCD, 2}, {"iret", 0xCF, 1},
		{"syscall", 0xF05, 2}, {"sysret", 0xF07, 2}, {"clc", 0xF8, 1}, {"stc", 0xF9, 1},
		{"cli", 0xFA, 1}, {"sti", 0xFB, 1}
	},
	// Data Movement
	{
		{"mov", 0x89, 2}, {"movzx", 0xB6, 3}, {"movsx", 0xBE, 3}, {"lea", 0x8D, 3},
		{"push", 0x50, 1}, {"pop", 0x58, 1}, {"xchg", 0x87, 2}, {"cmovz", 0x44, 3}
	},
	// Bitwise
	{
		{"and", 0x21, 2}, {"or", 0x09, 2}, {"xor", 0x31, 2}, {"not", 0xF7, 2},
		{"shl", 0xD3, 2}, {"shr", 0xD1, 2}, {"sal", 0xD3, 2}, {"sar", 0xD1, 2}
	},
	// Stack
	{
		{"pushf", 0x9C, 1}, {"popf", 0x9D, 1}, {"pusha", 0x60, 1}, {"popa", 0x61, 1},
		{"enter", 0xC8, 3}, {"leave", 0xC9, 1}
	}
};

// TODO: Multiplatform me

typedef struct {
	void* MaximumApplicationAddress;
	void* MinimumApplicationAddress;
	unsigned long NumOfProcessors;

} SystemInformation;

class ByteWalker
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="runtime"></param>
	ByteWalker(MonoStrataRuntime* Primary);

	/// <summary>
	/// Disassembles the method in memory, returning the full set of assembly instructions
	/// </summary>
	/// <param name="method"></param>
	/// <returns>std::string</returns>
	std::string GetDisassembly(void* method);

	/// <summary>
	/// Returns size of method
	/// </summary>
	/// <param name="method"></param>
	/// <returns></returns>
	size_t GetMethodSize(void* method);

	void* GetMaximumApplicationAddress();

	void* GetMinimumApplicationAddress();

	/// <summary>
	/// Deconstructor
	/// </summary>
	~ByteWalker();
private:
	size_t GetRoughMethodSize(void* method);
	Opcode Disassemble(BYTE* code);
	void SetSystemInfo();

	MonoStrataRuntime* Primary;
	SystemInformation* sysInfo;
};

