#pragma once

#include <cstdint>
#include <memory>

#include "../../utilities/_cxx17.h"
#include "../Stream.h"
#include "CompressionMode.h"

namespace common_lib {
    namespace io {
        namespace compression {

            struct IDeflater;
            struct IInflater;

            struct DeflateStream final : public Stream {

                DECLARE_CLASS(DeflateStream, Stream);

            private:

                std::shared_ptr<Stream> _baseStream;
                CompressionMode _mode;
                int32_t _level;
                IDeflater *_deflater;
                IInflater *_inflater;

                uint8_t *_buffer;
                size_t _bufferSize;

                bool _wroteBytes;

            public:

                explicit DeflateStream(const std::shared_ptr<Stream> &stream, CompressionMode mode);

                explicit DeflateStream(const std::shared_ptr<Stream> &stream, CompressionMode mode, int32_t level);

                ~DeflateStream() override;

                _CXX17_ATTR_NODISCARD
                std::shared_ptr<Stream> getBaseStream() const;

                _CXX17_ATTR_NODISCARD
                CompressionMode getCompressionMode() const;

                _CXX17_ATTR_NODISCARD
                int32_t getCompressionLevel() const;

                int32_t read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

                int32_t write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

                void flush() override;

                _CXX17_ATTR_NODISCARD
                bool canRead() const override;

                _CXX17_ATTR_NODISCARD
                bool canWrite() const override;

            private:

                void initialize();

                void purgeBuffers();

                int32_t writeDeflaterOutput();

            };

        }
    }
}
