#include <stdexcept>
#include <cinttypes>
#include <zlib.h>

#include "../../../utilities/StrHelper.h"
#include "zlib_debug.h"

typedef int z_error_code;

using namespace common_lib::utilities;

void ThrowDefaultZLibError(const char *stage, const char *description, z_error_code ret) noexcept(false) {
    if (description == nullptr) {
        throw std::runtime_error(StrHelper::format("zlib: @%s: %" PRId32, stage, ret));
    } else {
        throw std::runtime_error(StrHelper::format("zlib: @%s: %s (%" PRId32 ")", stage, description, ret));
    }
}

void ThrowCommonZLibError(const char *stage, z_error_code ret, bool ignoreBufError) noexcept(false) {
    switch (ret) {
        case Z_OK:
            break;
        case Z_STREAM_END:
            break;
        case Z_NEED_DICT:
            ThrowDefaultZLibError(stage, "need dictionary", ret);
            break;
        case Z_ERRNO:
            ThrowDefaultZLibError(stage, "general error", ret);
            break;
        case Z_STREAM_ERROR:
            ThrowDefaultZLibError(stage, "stream error", ret);
            break;
        case Z_DATA_ERROR:
            ThrowDefaultZLibError(stage, "data error", ret);
            break;
        case Z_MEM_ERROR:
            ThrowDefaultZLibError(stage, "memory error", ret);
            break;
        case Z_BUF_ERROR:
            // Under some circumstances, it is recoverable
            if (!ignoreBufError) {
                ThrowDefaultZLibError(stage, "buffer error", ret);
            }
            break;
        case Z_VERSION_ERROR:
            ThrowDefaultZLibError(stage, "version error", ret);
            break;
        default:
            ThrowDefaultZLibError(stage, nullptr, ret);
            break;
    }
}
