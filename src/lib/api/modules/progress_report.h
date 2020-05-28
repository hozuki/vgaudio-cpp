#pragma once

#include <memory>

struct _vga_progress_reporter;

namespace vgaudio {
    struct IProgressReport;
}

namespace vgaudio {
    namespace api {

        std::shared_ptr<vgaudio::IProgressReport> CreateProgressReporter(const struct _vga_progress_reporter *reporter, const void *reporterData);

    }
}
