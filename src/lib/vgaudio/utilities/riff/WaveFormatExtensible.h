#pragma once

#include <cstdint>

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/Guid.h"

namespace common_lib {
    namespace io {
        struct BinaryReader;
    }
}

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffParser;

            struct WaveFormatExtensible final {

            private:

                int32_t _size;
                int32_t _validBitsPerSample;
                uint32_t _channelMask;
                common_lib::utilities::Guid _subFormat;
                common_lib::utilities::array_ptr<uint8_t> _extra;

            public:

                ~WaveFormatExtensible() = default;

                static std::shared_ptr<WaveFormatExtensible> parse(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                _CXX17_ATTR_NODISCARD
                int32_t getSize() const;

                void setSize(int32_t size);

                _CXX17_ATTR_NODISCARD
                int32_t getValidBitsPerSample() const;

                void setValidBitsPerSample(int32_t bits);

                _CXX17_ATTR_NODISCARD
                int32_t getSamplesPerBlock() const;

                void setSamplesPerBlock(int32_t samples);

                _CXX17_ATTR_NODISCARD
                uint32_t getChannelMask() const;

                void setChannelMask(uint32_t mask);

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::Guid getSubFormat() const;

                void setSubFormat(const common_lib::utilities::Guid &format);

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::array_ptr<uint8_t> getExtra() const;

                void setExtra(const common_lib::utilities::array_ptr<uint8_t> &extra);

            private:

                explicit WaveFormatExtensible(const std::shared_ptr<common_lib::io::BinaryReader> &reader);

            };

        }
    }
}
