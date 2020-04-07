#pragma once

#include <stdint.h>

#include "../private/dll_public.h"

struct vga_byte_buffer;

VGA_C_API_DECL(void) vgaUtilDeflateData(const void *data, size_t dataSize, size_t bufferSize, struct vga_byte_buffer *output, size_t *outputSize);

VGA_C_API_DECL(void) vgaUtilInflateData(const void *data, size_t dataSize, size_t bufferSize, struct vga_byte_buffer *output, size_t *outputSize);
