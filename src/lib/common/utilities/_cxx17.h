#pragma once

#if defined(_MSC_VER) && _MSVC_LANG >= 201703L

#define _CXX17_AVAILABLE (1)

#elif defined(__cplusplus) && __cplusplus >= 201703L

#define _CXX17_AVAILABLE (1)

#else

#define _CXX17_AVAILABLE (0)

#endif

#if _CXX17_AVAILABLE

#define _CXX17_ATTR_MAYBE_UNUSED [[maybe_unused]]
#define _CXX17_ATTR_NODISCARD [[nodiscard]]

#define _CXX17_CONSTEXPR constexpr

#else

#define _CXX17_AVAILABLE (0)

#define _CXX17_ATTR_MAYBE_UNUSED
#define _CXX17_ATTR_NODISCARD

#define _CXX17_CONSTEXPR

#endif
