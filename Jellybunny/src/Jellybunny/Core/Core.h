#pragma once

#include <memory>

#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
	/* Windows x64  */
	#define JB_PLATFORM_WINDOWS
#else
	/* Windows x86 */
	#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
	#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
	#define JB_PLATFORM_IOS
	#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
	#define JB_PLATFORM_MACOS
	#error "MacOS is not supported!"
#else
	#error "Unknown Apple platform!"
#endif
	/* We also have to check __ANDROID__ before __linux__
	* since android is based on the linux kernel
	* it has __linux__ defined */
#elif defined(__ANDROID__)
	#define JB_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define JB_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif // End of platform detection

#ifdef JB_DEBUG
	#if defined(JB_PLATFORM_WINDOWS)
		#define JB_DEBUGBREAK() __debugbreak()
	#elif defined(JB_PLATFORM_LINUX)
		#include <signal.h>
		#define JB_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
		#define JB_ENABLE_ASSERTS
	#else
		#define JB_DEBUGBREAK()
#endif


#ifdef JB_ENABLE_ASS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define JB_INTERNAL_ASS_IMPL(type, check, msg, ...) { if(!(check)) { JB##type##ERROR(msg, __VA_ARGS__); JB_DEBUGBREAK(); } }
#define JB_INTERNAL_ASS_WITH_MSG(type, check, ...) JB_INTERNAL_ASS_IMPL(type, check, "ASSion failed: {0}", __VA_ARGS__)
#define JB_INTERNAL_ASS_NO_MSG(type, check) JB_INTERNAL_ASS_IMPL(type, check, "ASSion '{0}' failed at {1}:{2}", JB_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define JB_INTERNAL_ASS_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define JB_INTERNAL_ASS_GET_MACRO(...) JB_EXPAND_MACRO( JB_INTERNAL_ASS_GET_MACRO_NAME(__VA_ARGS__, JB_INTERNAL_ASS_WITH_MSG, JB_INTERNAL_ASS_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define JB_ASS(...) JB_EXPAND_MACRO( JB_INTERNAL_ASS_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define JB_CORE_ASS(...) JB_EXPAND_MACRO( JB_INTERNAL_ASS_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define JB_ASS(...)
#define JB_CORE_ASS(...)
#endif

#define BIT(x) (1 << x)
#define JB_BIND_EVENT_FN(fn) [this](auto&& ...args) -> decltype(auto){ return this->fn(std::forward<decltype(args)>(args)...); }

namespace Jellybunny
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}