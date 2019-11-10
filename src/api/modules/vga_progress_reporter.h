#pragma once

#include <stdint.h>

typedef void (*vga_progress_reporter_report_func_t)(int32_t value, const void *userData);

typedef void (*vga_progress_reporter_report_add_func_t)(int32_t value, const void *userData);

typedef void (*vga_progress_reporter_set_total_func_t)(int32_t value, const void *userData);

typedef void (*vga_progress_reporter_log_message_func_t)(const char *message, const void *userData);

typedef struct _vga_progress_reporter {

    vga_progress_reporter_report_func_t report;
    vga_progress_reporter_report_add_func_t reportAdd;
    vga_progress_reporter_set_total_func_t setTotal;
    vga_progress_reporter_log_message_func_t logMessage;

} vga_progress_reporter;
