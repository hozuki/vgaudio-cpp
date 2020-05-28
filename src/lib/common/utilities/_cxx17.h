#pragma once

#if defined(__cplusplus) && __cplusplus >= 201703L

#define _CXX17_AVAILABLE (1)

#define _CXX17_MAYBE_UNUSED [[maybe_unused]]

#else

#define _CXX17_AVAILABLE (0)

#define _CXX17_MAYBE_UNUSED

#endif
