#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "../../../common/utilities/type_sys.h"
#include "../../../common/utilities/runtime_array.h"

namespace common_lib::io {
    struct BinaryReader;
}

namespace vgaudio::utilities::riff {

    struct RiffSubChunk {

        DECLARE_ROOT_CLASS(RiffSubChunk);

    private:

        std::string _subChunkId;
        int32_t _subChunkSize;
        common_lib::utilities::array_ptr<uint8_t> _extra;

    public:

        explicit RiffSubChunk(const std::shared_ptr<common_lib::io::BinaryReader> &reader);

        virtual ~RiffSubChunk() = default;

        [[nodiscard]]
        std::string getSubChunkId() const;

        void setSubChunkId(const std::string &id);

        [[nodiscard]]
        int32_t getSubChunkSize() const;

        void setSubChunkSize(int32_t size);

        [[nodiscard]]
        common_lib::utilities::array_ptr<uint8_t> getExtra() const;

        void setExtra(const common_lib::utilities::array_ptr<uint8_t> &extra);

    };

}
