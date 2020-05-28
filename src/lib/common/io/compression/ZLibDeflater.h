#pragma once

#include <cstdint>
#include <memory>

#include "../../utilities/_cxx17.h"
#include "../../utilities/type_sys.h"
#include "IDeflater.h"

struct z_stream_s;

namespace common_lib {
    namespace io {
        namespace compression {

            typedef int z_error_code;
            typedef int z_flush_code;

            struct ZLibDeflater : public IDeflater {

                DECLARE_ROOT_CLASS(ZLibDeflater);

            private:

                z_stream_s *_z;
                int32_t _level;

                uint8_t *_inputBuffer;
                size_t _inputBufferSize;

            public:

                explicit ZLibDeflater(int32_t level);

                ~ZLibDeflater() override;

                _CXX17_ATTR_NODISCARD
                bool needsInput() const override;

                void setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) override;

                int32_t getDeflateOutput(void *outputBuffer, size_t outputBufferSize) override;

                bool finish(void *outputBuffer, size_t outputBufferSize, int32_t *bytesRead) override;

            private:

                void initialize();

                z_error_code readDeflateOutput(void *outputBuffer, size_t outputBufferSize, z_flush_code flushCode, int32_t *bytesRead);

                void invokeDeflateInit(int32_t compressionLevel, int32_t windowBits, int32_t memoryLevel, int32_t strategy);

                z_error_code invokeDeflate(z_flush_code flushCode) noexcept(false);

                void recreateInputBuffer(size_t size);

                void disposeInputBuffer();

                _CXX17_ATTR_NODISCARD
                bool isInputBufferCreated() const;

            };

        }
    }
}
