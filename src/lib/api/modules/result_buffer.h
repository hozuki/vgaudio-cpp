#pragma once

#include <cstdint>

struct result_buffer;

size_t AllocResultBuffer(struct result_buffer *buffer, size_t size);
