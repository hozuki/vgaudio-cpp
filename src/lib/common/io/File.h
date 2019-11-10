#pragma once

#include <string>

#include "../utilities/static_class.h"

namespace common_lib::io {

    using namespace std;

    struct File {

        IMPLEMENT_STATIC_CLASS(File);

    public:

        static bool exists(const string &path);

        static bool exists(const char *path);

    };

}
