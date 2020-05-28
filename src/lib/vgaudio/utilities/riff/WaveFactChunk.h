#pragma once

#include "RiffSubChunk.h"

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffParser;

            struct WaveFactChunk : public RiffSubChunk {

                DECLARE_CLASS(WaveFactChunk, RiffSubChunk);

            private:

                int32_t _sampleCount;

            public:

                ~WaveFactChunk() override = default;

                static std::shared_ptr<WaveFactChunk> parse(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                [[nodiscard]]
                int32_t getSampleCount() const;

                void setSampleCount(int32_t sampleCount);

            protected:

                WaveFactChunk(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

            };

        }
    }
}
