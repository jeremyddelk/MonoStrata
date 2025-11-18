#pragma once

class MonoStrataRuntime;

#define MONO_TABLE_TYPEDEF 0x02
#define MONO_TYPEDEF_SIZE 6
#define MONO_TYPEDEF_NAMESPACE 0
#define MONO_TYPEDEF_NAME 1

typedef struct {
	const uint8_t* base;
	uint32_t rows;
} MonoTableInfo;

typedef struct {
	std::string methodname;
	void* methodPtr;
} MonoMethod;

typedef void MonoAssembly;
typedef void MonoImage;

typedef struct {
	std::string assemblyName;
	MonoAssembly* assembly;
} Assembly;

// ======* Root/Image *===== //
typedef void* (__cdecl* mono_get_root_domain_func)();
typedef void* MonoDomain;
typedef void* (*mono_assembly_get_image_func)(void* assembly);
typedef const char* (*mono_image_get_name_func)(void* image);

// ======* Metadata Tables *===== //
typedef MonoTableInfo* (*mono_image_get_table_info_func)(void* image, int table_id);
typedef int (*mono_table_info_get_rows_func)(MonoTableInfo* table);
typedef void (*mono_metadata_decode_row_func)(const MonoTableInfo* table, int idx, uint32_t* res, int size);

// ======* String Heap *===== //
typedef const char* (*mono_metadata_string_heap_func)(void* image, uint32_t index);

// ======* Getters *===== //
typedef void* (*mono_class_from_name_func)(void* image, const char* name_space, const char* name);
typedef void* (*mono_class_get_methods_func)(void* klass, void** iter);
typedef const char* (*mono_method_get_name_func)(void* method);

// ======* Foreach *===== //
typedef void* (*mono_assembly_foreach_func)(void (*callback)(void* assembly, void* user_data), void* user_data);

struct Assemblies
{
	std::vector<std::unique_ptr<Assembly>> list;

	bool empty() const
	{
		return list.empty();
	}

	size_t size() const
	{
		return list.size();
	}

	void AddAssembly(const std::string& name, MonoAssembly* asmPtr)
	{
		list.push_back(std::make_unique<Assembly>(Assembly{ name, asmPtr }));
	}

	void clear()
	{
		list.clear();
	}

	auto begin() const { return list.begin(); }
	auto end() const { return list.end(); }
};

struct MonoRuntime
{
	// ============* Mono Entry *=========== //
	//
	//======* Name *===== //
	std::string name;
	//
	// ======* Library Handle *===== //
	LIB_HANDLE monoLib = nullptr;
	//
	// ======* Mono Domain *===== //
	MonoDomain Domain = nullptr;
	Assemblies Assemblies;
	//
	// ======* Root/Image *===== //
	mono_get_root_domain_func GetRootDomain = nullptr;
	mono_assembly_get_image_func GetAssemblyImage = nullptr;
	mono_image_get_name_func GetImageName = nullptr;
	//
	// ======* Metadata Tables *===== //
	mono_image_get_table_info_func GetTableInfo = nullptr;
	mono_table_info_get_rows_func GetTableRows = nullptr;
	mono_metadata_decode_row_func DecodeRow = nullptr;
	//
	// ======* String Heap *===== //
	mono_metadata_string_heap_func GetStringHeap = nullptr;
	//
	// ======* Getters *===== //
	mono_class_from_name_func GetClassFromName = nullptr;
	mono_class_get_methods_func GetMethodFromClass = nullptr;
	mono_method_get_name_func GetMethodName = nullptr;
	//
	// ======* Foreach *===== //
	mono_assembly_foreach_func ForEach = nullptr;

	bool CheckValidity()
	{
		return (GetRootDomain && GetAssemblyImage && GetImageName &&
			GetTableInfo && GetTableRows && DecodeRow &&
			GetStringHeap && GetClassFromName &&
			GetMethodFromClass && GetMethodName && ForEach);
	}

	void LoadUserAssemblies()
	{
		if (!ForEach || !GetAssemblyImage || !GetImageName)
			return;

		Assemblies.clear();

		ForEach([](MonoAssembly* assembly, void* user_data) {
			MonoRuntime* rt = reinterpret_cast<MonoRuntime*>(user_data);
			if (!rt || !assembly) return;

			MonoImage* image = rt->GetAssemblyImage(assembly);
			if (!image) return;

			const char* name = rt->GetImageName(image);
			if (!name) return;

			if (strstr(name, "Assembly-CSharp") || strstr(name, "Assembly-CSharp-Editor"))
			{
				rt->Assemblies.AddAssembly(name, assembly);
			}
		}, this);
	}
};




struct Runtimes
{
	std::vector<std::unique_ptr<MonoRuntime>> list;
	

	bool empty() const
	{
		return list.empty();
	}

	size_t size() const
	{
		return list.size();
	}

	void AddRuntime(const std::string& name, LIB_HANDLE asmPtr)
	{
		auto runtime = std::make_unique<MonoRuntime>();
		runtime->name = name;
		runtime->monoLib = asmPtr;

		// ============* Mono Entry *=========== //
		// 
		// ======* Root/Image *===== //
		BIND_MONO_FUNC(runtime->monoLib, mono_get_root_domain, runtime->GetRootDomain);
		//
		if (runtime->GetRootDomain)
		{
			runtime->Domain = runtime->GetRootDomain();

			if (!runtime->Domain)
			{
				throw new std::runtime_error("Root Domain Failure" + name);
			}
		}
		//
		BIND_MONO_FUNC(runtime->monoLib, mono_assembly_get_image, runtime->GetAssemblyImage);
		BIND_MONO_FUNC(runtime->monoLib, mono_image_get_name, runtime->GetImageName);
		//
		// ======* Metadata Tables *===== //
		BIND_MONO_FUNC(runtime->monoLib, mono_image_get_table_info, runtime->GetTableInfo);
		BIND_MONO_FUNC(runtime->monoLib, mono_table_info_get_rows, runtime->GetTableRows);
		BIND_MONO_FUNC(runtime->monoLib, mono_metadata_decode_row, runtime->DecodeRow);
		//
		// ======* String Heap *===== //
		BIND_MONO_FUNC(runtime->monoLib, mono_metadata_string_heap, runtime->GetStringHeap);
		//
		// ======* Getters *===== //
		BIND_MONO_FUNC(runtime->monoLib, mono_class_from_name, runtime->GetClassFromName);
		BIND_MONO_FUNC(runtime->monoLib, mono_class_get_methods, runtime->GetMethodFromClass);
		BIND_MONO_FUNC(runtime->monoLib, mono_method_get_name, runtime->GetMethodName);
		//
		// ======* Foreach *===== //
		BIND_MONO_FUNC(runtime->monoLib, mono_assembly_foreach, runtime->ForEach);

		if (runtime->CheckValidity())
		{
			list.push_back(std::move(runtime));
		}
		else
			throw new std::runtime_error("MonoRuntime Init Failure: " + name);
	}

	void clear()
	{
		for (auto& runtime : list) {
			if (runtime->monoLib) {
				FreeLibrary(runtime->monoLib);
			}
		}
		list.clear();
	}

	auto begin() const { return list.begin(); }
	auto end() const { return list.end(); }
};

struct AssemblyScanContext
{
	MonoRuntime* runtime;
	MonoAssembly* foundAssembly;
	const char* filePath;
};

/// <summary>
/// Root Domain Class For Mono JIT
/// </summary>
class MonoStrataRootDomain
{
public:
	Runtimes monoRuntimes;

	bool initialized;

	/// <summary>
	/// Constructor
	/// </summary>
	MonoStrataRootDomain();

	/// <summary>
	/// Returns the static singleton instance
	/// </summary>
	/// <returns></returns>
	static MonoStrataRootDomain* GetInstance();


	/// <summary>
	/// Deconstructor
	/// </summary>
	~MonoStrataRootDomain();

private:
	static MonoStrataRootDomain* instance;

	std::vector<MonoMethod> g_methodList;

	/// <summary>
	/// Seeks and finds Mono.dll module
	/// </summary>
	/// <returns>std::string</returns>
	static std::vector<std::string> GetMonoModule();
};