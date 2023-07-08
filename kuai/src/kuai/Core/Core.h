#pragma once

// @cond

#include <memory>

#define KU_ENABLE_ASSERTS // TODO: add to premake for debug configurations

#ifdef KU_PLATFORM_WINDOWS
	#define DEBUG_BREAK __debugbreak()
#else
	#include <signal.h>
	#define DEBUG_BREAK raise(SIGTRAP)
#endif

#ifdef KU_ENABLE_ASSERTS
	#define KU_ASSERT(x, ...) if(!(x)) { KU_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; }
	#define KU_CORE_ASSERT(x, ...) if(!(x)) { KU_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; }
#else
	#define KU_ASSERT(x, ...) 
	#define KU_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

namespace kuai {
	// Abbreviations for integer types
	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	// Rename shared_ptr to Rc (reference counter)
	template<typename T>
	using Rc = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Rc<T> makeRc(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Rename unique_ptr to Box
	template<typename T>
	using Box = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Box<T> makeBox(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}

// @endcond