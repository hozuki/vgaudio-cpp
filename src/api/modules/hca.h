#pragma once

#include <stdint.h>

#include "../private/dll_public.h"
#include "vga_progress_reporter.h"

typedef enum _CRIHCA_QUALITY {

    CRIHCA_QUALITY_NOT_SET = 0,
    CRIHCA_QUALITY_HIGHEST = 1,
    CRIHCA_QUALITY_HIGH = 2,
    CRIHCA_QUALITY_MIDDLE = 3,
    CRIHCA_QUALITY_LOW = 4,
    CRIHCA_QUALITY_LOWEST = 5,

} CRIHCA_QUALITY;

typedef struct _HCA_ENCODE_CONFIG {
    vga_progress_reporter *reporter;
    void *reporterData;
    CRIHCA_QUALITY quality;
    uint64_t key;
} HCA_ENCODE_CONFIG;

typedef struct _HCA_DECODE_CONFIG {
    vga_progress_reporter *reporter;
    void *reporterData;
    uint64_t key;
} HCA_DECODE_CONFIG;

VGA_C_API_DECL(int32_t) EncodeHcaFile(const char *inputWave, const char *outputHca, const HCA_ENCODE_CONFIG *config);

VGA_C_API_DECL(int32_t) DecodeHcaFile(const char *inputHca, const char *outputWave, const HCA_DECODE_CONFIG *config);
