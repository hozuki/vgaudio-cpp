#pragma once

#include <memory>
#include <string>

#include "../utilities/type_sys.h"
#include "../utilities/runtime_array.h"

namespace common_lib {
    namespace io {

        struct Stream;

        struct BinaryWriter {

            DECLARE_ROOT_CLASS(BinaryWriter);

        private:

            std::shared_ptr<Stream> _stream;

        public:

            explicit BinaryWriter(const std::shared_ptr<Stream> &stream);

            virtual ~BinaryWriter() = default;

            [[nodiscard]]
            std::shared_ptr<Stream> getBaseStream() const;

            virtual int32_t write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count);

            int32_t write(const common_lib::utilities::array_ptr<uint8_t> &buffer, int32_t count);

            int32_t write(const common_lib::utilities::array_ptr<uint8_t> &buffer, int32_t offset, int32_t count);

            virtual int32_t write(uint8_t value);

            virtual int32_t write(int8_t value);

            virtual int32_t write(uint16_t value);

            virtual int32_t write(int16_t value);

            virtual int32_t write(uint32_t value);

            virtual int32_t write(int32_t value);

            virtual int32_t write(uint64_t value);

            virtual int32_t write(int64_t value);

            virtual int32_t write(float value);

            virtual int32_t write(double value);

            virtual int32_t write(bool value);

            int32_t writeNTString(const char *str);

            /**
             * Write a null-terminated string with size limitation.
             * @param str The string to write
             * @param maxSize Size in bytes (null terminator included)
             * @return Number of bytes written
             */
            virtual int32_t writeNTString(const char *str, size_t maxSize);

            int32_t writeNTString(const std::string &str);

            virtual int32_t writeFixedString(const char *str, size_t size);

            int32_t writeFixedString(const std::string &str);

        };

    }
}
