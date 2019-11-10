#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>

#include "../utilities/defer.h"
#include "File.h"

using namespace std;
using namespace common_lib::io;
using namespace common_lib::utilities;

bool File::exists(const string &path) {
    return exists(path.c_str());
}

bool File::exists(const char *path) {
    auto fp = fopen(path, "rb");

    auto_defer([&]() {
        if (fp != nullptr) {
            fclose(fp);
        }
    });

    const auto r = fp != nullptr;

    return r;
}
