#pragma once

#include <cstdint>

#include "SeekOrigin.h"
#include "../utilities/runtime_array.h"
#include "../utilities/type_sys.h"

namespace common_lib::io {

    struct Stream {

        DECLARE_ROOT_CLASS(Stream);

    public:

        virtual ~Stream() = default;

        virtual int32_t read(void *buffer, size_t bufferSize, int32_t offset, int32_t count);

        virtual int32_t read(const common_lib::utilities::array_ptr<uint8_t> &buffer, int32_t offset, int32_t count);

        virtual int32_t write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count);

        virtual int32_t write(const common_lib::utilities::array_ptr<uint8_t> &buffer, int32_t offset, int32_t count);

        virtual void flush();

        virtual void seek(int64_t offset, SeekOrigin origin);

        virtual int64_t getPosition();

        virtual void setPosition(int64_t position);

        virtual int64_t getLength();

        virtual void setLength(int64_t length);

        [[nodiscard]]
        virtual bool canRead() const;

        [[nodiscard]]
        virtual bool canWrite() const;

        [[nodiscard]]
        virtual bool canSeek() const;

        void copyTo(const std::shared_ptr<Stream> &destination);

        void copyTo(const std::shared_ptr<Stream> &destination, size_t bufferSize);

    protected:

        Stream() = default;

        static void validateBufferParameters(const void *buffer, size_t bufferSize, int32_t offset, int32_t count);

    };

}
