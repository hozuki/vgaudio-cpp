#pragma once

#include <cstdint>
#include <memory>

#include "../utilities/_cxx17.h"
#include "../utilities/type_sys.h"
#include "Stream.h"
#include "../utilities/runtime_array.h"

namespace common_lib {
    namespace io {

        struct MemoryStream : public Stream {

            DECLARE_CLASS(MemoryStream, Stream);

        private:

            common_lib::utilities::array_ptr<uint8_t> _buffer;
            bool _writable;
            bool _canResize;
            int64_t _length;
            int64_t _position;

        public:

            MemoryStream();

            explicit MemoryStream(const common_lib::utilities::array_ptr<uint8_t> &buffer);

            explicit MemoryStream(size_t initialCapacity);

            MemoryStream(const common_lib::utilities::array_ptr<uint8_t> &buffer, bool writable);

            MemoryStream(const void *buffer, size_t count);

            MemoryStream(void *buffer, size_t count, bool writable);

            ~MemoryStream() override = default;

            _CXX17_ATTR_NODISCARD
            common_lib::utilities::array_ptr<uint8_t> toArray();

            int32_t read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

            int32_t write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

            void flush() override;

            int64_t getPosition() override;

            void setPosition(int64_t position) override;

            int64_t getLength() override;

            void setLength(int64_t length) override;

            _CXX17_ATTR_NODISCARD
            int64_t getCapacity() const;

            void setCapacity(int64_t capacity);

            _CXX17_ATTR_NODISCARD
            common_lib::utilities::array_ptr<uint8_t> getBuffer() const;

            _CXX17_ATTR_NODISCARD
            bool canRead() const override;

            _CXX17_ATTR_NODISCARD
            bool canWrite() const override;

            _CXX17_ATTR_NODISCARD
            bool canSeek() const override;

            void writeTo(const std::shared_ptr<Stream> &stream);

        private:

            void ensureCapacity(int64_t target, bool exact);

            void setPosition(int64_t position, bool direct);

        };

    }
}
