#include <cassert>

#include "../../common/utilities/TypeHelper.h"
#include "../../common/io/File.h"
#include "../../../api/modules/misc.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

VGA_API_IMPL(size_t)vgaDemangleSymbolName(const char *name, char *buffer, size_t bufferSize) {
    assert(name != nullptr);
    assert(buffer != nullptr);

    const auto str = TypeHelper::demangle(name);
    const auto size = str.size() + 1;

    if (bufferSize < size) {
        return static_cast<size_t>(-1);
    }

    memcpy(buffer, str.c_str(), size);
    buffer[size - 1] = '\0';

    return size;
}

VGA_API_IMPL(vga_bool) vgaFileExists(const char *filePath) {
    return File::exists(filePath);
}
