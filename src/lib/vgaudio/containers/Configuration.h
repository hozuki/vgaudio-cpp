#pragma once

#include <memory>

#include "../../common/utilities/type_sys.h"

namespace vgaudio {
    struct IProgressReport;
}

namespace vgaudio::containers {

    using namespace std;
    using namespace vgaudio;

    struct Configuration {

        DECLARE_ROOT_CLASS(Configuration);

        Configuration();

        Configuration(const Configuration &other) = default;

        virtual ~Configuration() = default;

        shared_ptr<IProgressReport> progress;

        bool trimFile;

    };

}
