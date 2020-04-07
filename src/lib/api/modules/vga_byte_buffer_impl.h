#pragma once

#include <cstdint>

struct vga_byte_buffer;

size_t vgaUtilAllocByteBuffer(struct vga_byte_buffer *buffer, size_t size);
