#pragma once

#include <stdint.h>

#include "../private/dll_public.h"
#include "../private/vga_bool.h"

VGA_C_API_DECL(size_t)vgaUtilDemangleSymbolName(const char *name, char *buffer, size_t bufferSize);

VGA_C_API_DECL(vga_bool) vgaUtilFileExists(const char *filePath);
