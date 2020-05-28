#pragma once

#include <string>
#include <memory>

#include "static_class.h"

namespace common_lib {
    namespace utilities {

        struct StrHelper {

            IMPLEMENT_STATIC_CLASS(StrHelper);

            // https://stackoverflow.com/a/26221725
            template<typename ... Args>
            static std::string format(const std::string &format, Args... args) {
                size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
                std::unique_ptr<char[]> buf(new char[size]);
                snprintf(buf.get(), size, format.c_str(), args ...);
                return std::string(buf.get(), buf.get() + size - 1);
            }

        };

    }
}
