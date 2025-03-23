#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"


namespace Jellybunny
{
	class Log
	{
	public:
		static void Init();

		static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static inline std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// CORE LOG MACROS

#define JB_CORE_FAT(...)    ::Jellybunny::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define JB_CORE_ERR(...)    ::Jellybunny::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JB_CORE_WAR(...)    ::Jellybunny::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JB_CORE_IFO(...)    ::Jellybunny::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JB_CORE_TRC(...)    ::Jellybunny::Log::GetCoreLogger()->trace(__VA_ARGS__)


// CLIENT LOG MACROS

#define JB_FAT(...)    ::Jellybunny::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define JB_ERR(...)    ::Jellybunny::Log::GetClientLogger()->error(__VA_ARGS__)
#define JB_WAR(...)    ::Jellybunny::Log::GetClientLogger()->warn(__VA_ARGS__)
#define JB_IFO(...)    ::Jellybunny::Log::GetClientLogger()->info(__VA_ARGS__)
#define JB_TRC(...)    ::Jellybunny::Log::GetClientLogger()->trace(__VA_ARGS__)

// if dist build
//#define JB_CORE_INFO