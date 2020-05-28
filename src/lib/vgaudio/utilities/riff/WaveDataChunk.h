#pragma once

#include "../../../common/utilities/_cxx17.h"
#include "RiffSubChunk.h"

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffParser;

            struct WaveDataChunk : public RiffSubChunk {

                DECLARE_CLASS(WaveDataChunk, RiffSubChunk);

            private:

                common_lib::utilities::array_ptr<uint8_t> _data;

            public:

                ~WaveDataChunk() override = default;

                static std::shared_ptr<WaveDataChunk> parse(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::array_ptr<uint8_t> getData() const;

                void setData(const common_lib::utilities::array_ptr<uint8_t> &data);

            protected:

                WaveDataChunk(const std::shared_ptr<RiffParser> &parser, const std::shared_ptr<common_lib::io::BinaryReader> &reader);

            };

        }
    }
}
