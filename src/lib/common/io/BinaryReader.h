#pragma once

#include <memory>
#include <string>

#include "Stream.h"
#include "../utilities/type_sys.h"

namespace common_lib::io {

    struct BinaryReader {

        DECLARE_ROOT_CLASS(BinaryReader);

    private:

        shared_ptr<Stream> _stream;

    public:

        explicit BinaryReader(const shared_ptr<Stream> &stream);

        virtual ~BinaryReader() = default;

        [[nodiscard]]
        shared_ptr<Stream> getBaseStream() const;

        virtual int32_t readBytes(void *buffer, size_t bufferSize, int32_t offset, int32_t count);

        int32_t readBytes(const array_ptr<uint8_t> &buffer, int32_t count);

        int32_t readBytes(const array_ptr<uint8_t> &buffer, int32_t offset, int32_t count);

        array_ptr<uint8_t> readBytes(int32_t count);

        virtual uint8_t readUInt8();

        virtual int8_t readInt8();

        virtual uint16_t readUInt16();

        virtual int16_t readInt16();

        virtual uint32_t readUInt32();

        virtual int32_t readInt32();

        virtual uint64_t readUInt64();

        virtual int64_t readInt64();

        virtual float readSingle();

        virtual double readDouble();

        virtual bool readBoolean();

        virtual std::string readNTString();

        virtual std::string readFixedString(size_t size);

    };

}
