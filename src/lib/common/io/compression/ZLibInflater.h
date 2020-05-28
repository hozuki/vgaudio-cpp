#pragma once

#include <cstdint>
#include <memory>

#include "../../utilities/_cxx17.h"
#include "../../utilities/type_sys.h"
#include "IInflater.h"

struct z_stream_s;

namespace common_lib {
    namespace io {
        namespace compression {

            typedef int z_error_code;
            typedef int z_flush_code;

            struct ZLibInflater : public IInflater {

                DECLARE_ROOT_CLASS(ZLibInflater);

            private:

                z_stream_s *_z;

                uint8_t *_inputBuffer;
                size_t _inputBufferSize;
                bool _isFinished;

            public:

                explicit ZLibInflater(int32_t windowBits);

                ~ZLibInflater() override;

                int32_t getAvailableOutput() override;

                _CXX17_ATTR_NODISCARD
                bool needsInput() const override;

                void setInput(const void *inputBuffer, size_t inputBufferSize, int32_t offset, int32_t count) override;

                int32_t getInflateOutput(void *outputBuffer, size_t outputBufferSize, int32_t offset, int32_t count) override;

                _CXX17_ATTR_NODISCARD
                bool isFinished() const override;

            private:

                void initialize(int32_t windowBits);

                void invokeInflateInit(int32_t windowBits);

                z_error_code invokeInflate(z_flush_code flushCode) noexcept(false);

                z_error_code readInflateOutput(void *outputBuffer, size_t outputBufferSize, int32_t offset, int32_t count, z_flush_code flushCode, int32_t *bytesRead);

                void recreateInputBuffer(size_t size);

                void disposeInputBuffer();

                _CXX17_ATTR_NODISCARD
                bool isInputBufferCreated() const;

            };

        }
    }
}
