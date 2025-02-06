#pragma once

#ifdef JB_PLATFORM_WINDOWS
	#ifdef JB_BUILD_DLL
		#define JELLYBUNNY_API __declspec(dllexport)
	#else
		#define JELLYBUNNY_API __declspec(dllimport)
	#endif
#else
	#error Jellybunny only supports Windows!
#endif

#ifdef JB_ENABLE_ASSERTS
	#define JB_ASSERT(x, ...) { if(!(x)) { JB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define JB_CORE_ASSERT(x, ...) { if(!(x)) { JB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define JB_ASSERT(x, ...)
	#define JB_CORE_ASS(x, ...)
#endif

#define BIT(x) (1 << x)

#define JB_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)