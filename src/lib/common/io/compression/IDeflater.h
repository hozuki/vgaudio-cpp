#pragma once

#include <cstdint>
#include <cstddef>

namespace common_lib {
    namespace io {
        namespace compression {

            struct IDeflater {

            public:

                virtual ~IDeflater() = default;

                [[nodiscard]]
                virtual bool needsInput() const = 0;

                virtual void setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) = 0;

                virtual int32_t getDeflateOutput(void *outputBuffer, size_t outputBufferSize) = 0;

                virtual bool finish(void *outputBuffer, size_t outputBufferSize, int32_t *bytesRead) = 0;

            protected:

                IDeflater() = default;

            };

        }
    }
}
