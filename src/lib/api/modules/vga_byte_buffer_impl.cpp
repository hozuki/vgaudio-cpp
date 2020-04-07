#include <cstdint>
#include <cstdlib>

#include "../../../api/modules/vga_byte_buffer.h"

struct vga_byte_buffer {

    uint8_t *p;
    size_t size;

    vga_byte_buffer()
        : p(nullptr), size(0) {
    }

};

VGA_API_IMPL(vga_byte_buffer *)vgaUtilCreateByteBuffer() {
    return new vga_byte_buffer();
}

VGA_API_IMPL(void) vgaUtilDestroyByteBuffer(vga_byte_buffer **pBuffer) {
    if (pBuffer && *pBuffer) {
        free((*pBuffer)->p);
        delete *pBuffer;
        *pBuffer = nullptr;
    }
}

VGA_API_IMPL(void *)vgaUtilGetByteBufferData(vga_byte_buffer *buffer) {
    if (buffer == nullptr) {
        return nullptr;
    }

    return buffer->p;
}

VGA_API_IMPL(size_t)vgaUtilGetByteBufferSize(vga_byte_buffer *buffer) {
    if (buffer == nullptr) {
        return 0;
    }

    return buffer->size;
}

size_t vgaUtilAllocByteBuffer(vga_byte_buffer *buffer, size_t size) {
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
