#pragma once

#include <cstdint>

#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/Guid.h"

namespace common_lib::io {
    struct BinaryReader;
}

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct RiffParser;

    struct WaveFormatExtensible final {

    private:

        int32_t _size;
        int32_t _validBitsPerSample;
        uint32_t _channelMask;
        Guid _subFormat;
        array_ptr<uint8_t> _extra;

    public:

        ~WaveFormatExtensible() = default;

        static shared_ptr<WaveFormatExtensible> parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

        [[nodiscard]]
        int32_t getSize() const;

        void setSize(int32_t size);

        [[nodiscard]]
        int32_t getValidBitsPerSample() const;

        void setValidBitsPerSample(int32_t bits);

        [[nodiscard]]
        int32_t getSamplesPerBlock() const;

        void setSamplesPerBlock(int32_t samples);

        [[nodiscard]]
        uint32_t getChannelMask() const;

        void setChannelMask(uint32_t mask);

        [[nodiscard]]
        Guid getSubFormat() const;

        void setSubFormat(const Guid &format);

        [[nodiscard]]
        array_ptr<uint8_t> getExtra() const;

        void setExtra(const array_ptr<uint8_t> &extra);

    private:

        explicit WaveFormatExtensible(const shared_ptr<BinaryReader> &reader);

    };

}
