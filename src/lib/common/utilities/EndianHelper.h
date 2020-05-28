#pragma once

#include <memory>

#include "endian.h"
#include "static_class.h"

namespace common_lib {
    namespace io {
        struct Stream;
        struct BinaryReader;
        struct BinaryWriter;
    }
}

namespace common_lib {
    namespace utilities {

        struct EndianHelper {

            IMPLEMENT_STATIC_CLASS(EndianHelper);

            static std::shared_ptr<common_lib::io::BinaryReader> createBinaryReader(const std::shared_ptr<common_lib::io::Stream> &stream, Endianess endian);

            static std::shared_ptr<common_lib::io::BinaryWriter> createBinaryWriter(const std::shared_ptr<common_lib::io::Stream> &stream, Endianess endian);

        };

    }
}
