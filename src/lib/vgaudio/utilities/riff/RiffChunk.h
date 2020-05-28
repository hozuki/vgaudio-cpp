#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "../../../common/utilities/_cxx17.h"

namespace common_lib {
    namespace io {
        struct BinaryReader;
    }
}

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffChunk final {

            private:

                std::string _chunkId;
                int32_t _size;
                std::string _type;

            private:

                RiffChunk();

            public:

                ~RiffChunk() = default;

                static std::shared_ptr<RiffChunk> parse(const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                _CXX17_ATTR_NODISCARD
                std::string getChunkId() const;

                void setChunkId(const std::string &id);

                _CXX17_ATTR_NODISCARD
                int32_t getSize() const;

                void setSize(int32_t size);

                _CXX17_ATTR_NODISCARD
                std::string getType() const;

                void setType(const std::string &type);

            };

        }
    }
}
