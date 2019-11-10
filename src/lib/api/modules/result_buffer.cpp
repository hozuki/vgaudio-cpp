#include <cstdint>
#include <cstdlib>

#include "../../../api/modules/result_buffer.h"

struct result_buffer {

    uint8_t *p;
    size_t size;

    result_buffer()
        : p(nullptr), size(0) {
    }

};

VGA_API_IMPL(struct result_buffer *)CreateResultBuffer() {
    return new result_buffer();
}

VGA_API_IMPL(void) DestroyResultBuffer(struct result_buffer **pBuffer) {
    if (pBuffer && *pBuffer) {
        free((*pBuffer)->p);
        delete *pBuffer;
        *pBuffer = nullptr;
    }
}

VGA_API_IMPL(void *)GetResultBufferData(struct result_buffer *buffer) {
    return buffer->p;
}

VGA_API_IMPL(size_t)GetResultBufferSize(struct result_buffer *buffer) {
    return buffer->size;
}

size_t AllocResultBuffer(struct result_buffer *buffer, size_t size) {
    if (size == 0) {
        return 0;
    }

    if (buffer == nullptr) {
        return 0;
    }

    if (buffer->p != nullptr) {
        return 0;
    }

    buffer->p = static_cast<uint8_t *>(malloc(size));
    buffer->size = size;

    return size;
}
