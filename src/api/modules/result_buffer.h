#pragma once

#include <stdint.h>

#include "../private/dll_public.h"

struct result_buffer;

VGA_C_API_DECL(struct result_buffer *)CreateResultBuffer();

VGA_C_API_DECL(void) DestroyResultBuffer(struct result_buffer **pBuffer);

VGA_C_API_DECL(void *) GetResultBufferData(struct result_buffer *buffer);

VGA_C_API_DECL(size_t) GetResultBufferSize(struct result_buffer *buffer);
