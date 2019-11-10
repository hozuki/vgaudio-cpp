#pragma once

#include <stdint.h>

#include "../private/dll_public.h"

struct result_buffer;

VGA_C_API_DECL(void) DeflateData(const void *data, size_t dataSize, size_t bufferSize, struct result_buffer *output, size_t *outputSize);

VGA_C_API_DECL(void) InflateData(const void *data, size_t dataSize, size_t bufferSize, struct result_buffer *output, size_t *outputSize);
