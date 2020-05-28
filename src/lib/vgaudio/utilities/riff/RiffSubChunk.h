#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/type_sys.h"
#include "../../../common/utilities/runtime_array.h"

namespace common_lib {
    namespace io {
        struct BinaryReader;
    }
}

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffSubChunk {

                DECLARE_ROOT_CLASS(RiffSubChunk);

            private:

                std::string _subChunkId;
                int32_t _subChunkSize;
                common_lib::utilities::array_ptr<uint8_t> _extra;

            public:

                explicit RiffSubChunk(const std::shared_ptr<common_lib::io::BinaryReader> &reader);

                virtual ~RiffSubChunk() = default;

                _CXX17_ATTR_NODISCARD
                std::string getSubChunkId() const;

                void setSubChunkId(const std::string &id);

                _CXX17_ATTR_NODISCARD
                int32_t getSubChunkSize() const;

                void setSubChunkSize(int32_t size);

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::array_ptr<uint8_t> getExtra() const;

                void setExtra(const common_lib::utilities::array_ptr<uint8_t> &extra);

            };

        }
    }
}
