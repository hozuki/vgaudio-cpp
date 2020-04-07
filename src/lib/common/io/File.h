#pragma once

#include <string>

#include "../utilities/static_class.h"

namespace common_lib::io {

    struct File {

        IMPLEMENT_STATIC_CLASS(File);

    public:

        static bool exists(const std::string &path);

        static bool exists(const char *path);

    };

}
