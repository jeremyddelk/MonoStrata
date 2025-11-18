#pragma once

class MonoStrataRuntime;

class MonoPatchingService
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="domain"></param>
	MonoPatchingService(MonoStrataRuntime* Primary);

	/// <summary>
	/// Reroutes method entirely to a different executable region in memory
	/// </summary>
	/// <param name="target"></param>
	/// <param name="hook"></param>
	/// <returns>bool</returns>
	bool PatchMethod(unsigned long long targetHash, unsigned long long hookHash);

	/// <summary>
	/// Restores PatchMethod
	/// </summary>
	/// <param name="target"></param>
	/// <param name="originalBytes"></param>
	/// <returns>bool</returns>
	bool RestorePatchMethod(void* target, BYTE* originalBytes);

	/// <summary>
	/// Deconstructor
	/// </summary>
	~MonoPatchingService();
private:
	MonoStrataRuntime* Primary;
};

