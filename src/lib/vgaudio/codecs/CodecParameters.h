#pragma once

#include <cstdint>
#include <memory>

#include "../../common/utilities/type_sys.h"

namespace vgaudio {
    struct IProgressReport;
}

namespace vgaudio {
    namespace codecs {

        struct CodecParameters {

            DECLARE_ROOT_CLASS(CodecParameters);

        public:

            CodecParameters();

            virtual ~CodecParameters() = default;

            std::shared_ptr<IProgressReport> progress;
            int32_t sampleCount;

        protected:

            CodecParameters(const CodecParameters &other) = default;

        };

    }
}
