#pragma once

namespace common_lib::utilities {

    enum class Endianess {

        BigEndian = 0,
        LittleEndian = 1

    };

    bool machine_is_big_endian();

    Endianess get_machine_endian();

}
