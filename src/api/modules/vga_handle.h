#pragma once

#include "../private/dll_public.h"
#include "../private/vga_bool.h"

struct vga_object;
typedef struct vga_object vga_object;

typedef vga_object *vga_handle;

VGA_C_API_DECL(vga_bool) vgaIsHandleValid(vga_handle handle);

VGA_C_API_DECL(void) vgaCloseHandle(vga_handle handle);
