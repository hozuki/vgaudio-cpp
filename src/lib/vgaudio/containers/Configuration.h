#pragma once

#include <memory>

#include "../../common/utilities/type_sys.h"

namespace vgaudio {
    struct IProgressReport;
}

namespace vgaudio {
    namespace containers {

        struct Configuration {

            DECLARE_ROOT_CLASS(Configuration);

            Configuration();

            Configuration(const Configuration &other) = default;

            virtual ~Configuration() = default;

            std::shared_ptr<vgaudio::IProgressReport> progress;

            bool trimFile;

        };

    }
}
