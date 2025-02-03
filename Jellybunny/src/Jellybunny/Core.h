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