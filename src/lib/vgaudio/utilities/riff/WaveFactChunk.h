#pragma once

#include "RiffSubChunk.h"

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct RiffParser;

    struct WaveFactChunk : public RiffSubChunk {

        DECLARE_CLASS(WaveFactChunk, RiffSubChunk);

    private:

        int32_t _sampleCount;

    public:

        ~WaveFactChunk() override = default;

        static shared_ptr<WaveFactChunk> parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

        [[nodiscard]]
        int32_t getSampleCount() const;

        void setSampleCount(int32_t sampleCount);

    protected:

        WaveFactChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

    };

}
