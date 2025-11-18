#pragma once
#include "pch.h"

class MonoPatchingService;
//class ByteWalker;


class MonoStrataRuntime
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	MonoStrataRuntime();

	void Start();

	/// <summary>
	/// Cleanup Operation on program
	/// </summary>
	void Cleanup();

	/// <summary>
	/// Deconstructor
	/// </summary>
	~MonoStrataRuntime();

	// =====* Program Components *===== //
	MonoPatchingService* PatchingService;
	//ByteWalker* Disassembler;
};

