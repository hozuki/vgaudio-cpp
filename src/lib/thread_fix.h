#pragma once

#if (defined(__MINGW32__) || defined(__MINGW64__)) && !defined(_POSIX_THREADS)

#include "../../ext/mingw-std-threads/mingw.thread.h"
#include "../../ext/mingw-std-threads/mingw.mutex.h"

#endif