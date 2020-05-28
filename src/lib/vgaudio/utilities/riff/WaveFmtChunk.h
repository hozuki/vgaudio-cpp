#pragma once

#include "../../../common/utilities/_cxx17.h"
#include "RiffSubChunk.h"

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffParser;
            struct WaveFormatExtensible;

            struct WaveFmtChunk : public RiffSubChunk {

                DECLARE_CLASS(WaveFmtChunk, RiffSubChunk);

            private:

                int32_t _formatTag;
                int32_t _channelCount;
                int32_t _sampleRate;
                int32_t _averageBytesPerSecond;
                int32_t _blockAlign;
                int32_t _bitsPerSample;
                std::shared_ptr<WaveFormatExtensible> _extensible;

            public:

                ~WaveFmtChunk() override = default;

                static std::shared_ptr<WaveFmtChunk> parse(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                _CXX17_ATTR_NODISCARD
                int32_t getFormatTag() const;

                void setFormatTag(int32_t tag);

                _CXX17_ATTR_NODISCARD
                int32_t getChannelCount() const;

                void setChannelCount(int32_t channelCount);

                _CXX17_ATTR_NODISCARD
                int32_t getSampleRate() const;

                void setSampleRate(int32_t sampleRate);

                _CXX17_ATTR_NODISCARD
                int32_t getAverageBytesPerSecond() const;

                void setAverageBytesPerSecond(int32_t bytesPerSecond);

                _CXX17_ATTR_NODISCARD
                int32_t getBlockAlign() const;

                void setBlockAlign(int32_t align);

                _CXX17_ATTR_NODISCARD
                int32_t getBitsPerSample() const;

                void setBitsPerSample(int32_t bitsPerSample);

                _CXX17_ATTR_NODISCARD
                std::shared_ptr<WaveFormatExtensible> getExtensible() const;

                void setExtensible(const std::shared_ptr<WaveFormatExtensible> &extensible);

            protected:

                WaveFmtChunk(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

            };

        }
    }
}
