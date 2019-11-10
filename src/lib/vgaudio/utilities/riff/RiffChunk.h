#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace common_lib::io {
    struct BinaryReader;
}

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::io;

    struct RiffChunk final {

    private:

        string _chunkId;
        int32_t _size;
        string _type;

    private:

        RiffChunk();

    public:

        ~RiffChunk() = default;

        static shared_ptr<RiffChunk> parse(const shared_ptr<BinaryReader> &reader);

        [[nodiscard]]
        string getChunkId() const;

        void setChunkId(const string &id);

        [[nodiscard]]
        int32_t getSize() const;

        void setSize(int32_t size);

        [[nodiscard]]
        string getType() const;

        void setType(const string &type);

    };

}
