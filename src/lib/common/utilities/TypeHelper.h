#pragma once

#include <string>

#if defined(__GNUG__)

#include <memory>
#include <cxxabi.h>

#endif

#include "static_class.h"

namespace common_lib::utilities {

    struct TypeHelper {

        IMPLEMENT_STATIC_CLASS(TypeHelper);

        template<typename TRef, typename TParam = void>
        static bool isType(TParam *ptr) {
            return static_cast<bool>(dynamic_cast<TRef *>(ptr));
        }

        template<typename TRef, typename TParam = void>
        static bool isType(const TParam *ptr) {
            return static_cast<bool>(dynamic_cast<const TRef *>(ptr));
        }

#if defined(__GNUG__)

        // https://stackoverflow.com/a/4541470
        static std::string demangle(const char *name) {
            int status = -4; // some arbitrary value to eliminate the compiler warning

            // enable c++11 by passing the flag -std=c++11 to g++
            std::unique_ptr<char, void (*)(void *)> res {
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free
            };

            return (status == 0) ? res.get() : name;
        }

#else

        static std::string demangle(const char *name) {
            return name;
        }

#endif

    };

}
