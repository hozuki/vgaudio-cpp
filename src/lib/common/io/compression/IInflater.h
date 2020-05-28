#pragma once

#include <cstdint>

namespace common_lib {
    namespace io {
        namespace compression {

            struct IInflater {

            public:

                virtual ~IInflater() = default;

                virtual int32_t getAvailableOutput() = 0;

                [[nodiscard]]
                virtual bool needsInput() const = 0;

                virtual void setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) = 0;

                virtual int32_t getInflateOutput(void *outputBuffer, size_t outputBufferSize, int32_t offset, int32_t count) = 0;

                [[nodiscard]]
                virtual bool isFinished() const = 0;

            protected:

                IInflater() = default;

            };

        }
    }
}
