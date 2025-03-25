#include "jbpch.h"
#include "ScriptModule.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Jellybunny
{
	struct ScriptModuleData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
	};

	static ScriptModuleData* s_Data = nullptr;

	void ScriptModule::Init()
	{
		s_Data = new ScriptModuleData();
		InitMono();
	}

	void ScriptModule::Die()
	{
		KillMono();
		delete s_Data;
	}

	char* ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}


	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			JB_CORE_TRC("{}.{}", nameSpace, name);
		}
	}

	void ScriptModule::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("JellybunnyJitRuntime");
		JB_CORE_ASS(rootDomain);

		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;

		// Create an App Domain
		s_Data->AppDomain = mono_domain_create_appdomain("JellybunnyScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);
		s_Data->CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Jellybunny-ScriptCore.dll");
		PrintAssemblyTypes(s_Data->CoreAssembly);

		MonoImage* assImage = mono_assembly_get_image(s_Data->CoreAssembly);
		MonoClass* monoClass = mono_class_from_name(assImage, "Jellybunny", "Main");

		MonoObject* obj = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(obj);

		MonoMethod* meth = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
		mono_runtime_invoke(meth, obj, nullptr, nullptr);

		MonoString* monoStr = mono_string_new(s_Data->AppDomain, "asdddsfdsdffad");
		MonoMethod* methCust2 = mono_class_get_method_from_name(monoClass, "PrintCustardMessage", 1);
		void* strParam = monoStr;
		mono_runtime_invoke(methCust2, obj, &strParam, nullptr);
	}

	void ScriptModule::KillMono()
	{
		// todo: not make it suck ass
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;
		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}
}