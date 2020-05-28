#pragma once

#include <cstdint>
#include <string>
#include <memory>

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

                [[nodiscard]]
                std::string getChunkId() const;

                void setChunkId(const std::string &id);

                [[nodiscard]]
                int32_t getSize() const;

                void setSize(int32_t size);

                [[nodiscard]]
                std::string getType() const;

                void setType(const std::string &type);

            };

        }
    }
}
