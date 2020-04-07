#pragma once

#include <stdint.h>

#include "../private/dll_public.h"

struct vga_byte_buffer;

typedef struct vga_byte_buffer vga_byte_buffer;

VGA_C_API_DECL(vga_byte_buffer *)vgaUtilCreateByteBuffer();

VGA_C_API_DECL(void) vgaUtilDestroyByteBuffer(vga_byte_buffer **pBuffer);

VGA_C_API_DECL(void *) vgaUtilGetByteBufferData(vga_byte_buffer *buffer);

VGA_C_API_DECL(size_t) vgaUtilGetByteBufferSize(vga_byte_buffer *buffer);
