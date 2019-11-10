#pragma once

#include "RiffSubChunk.h"

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct RiffParser;

    struct WaveDataChunk : public RiffSubChunk {

        DECLARE_CLASS(WaveDataChunk, RiffSubChunk);

    private:

        array_ptr<uint8_t> _data;

    public:

        ~WaveDataChunk() override = default;

        static shared_ptr<WaveDataChunk> parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

        [[nodiscard]]
        array_ptr<uint8_t> getData() const;

        void setData(const array_ptr<uint8_t> &data);

    protected:

        WaveDataChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

    };

}
