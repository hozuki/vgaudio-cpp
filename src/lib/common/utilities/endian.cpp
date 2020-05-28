#ifdef _WIN32

#include <winsock.h>

#else

#include <arpa/inet.h>

#endif

#include "endian.h"
#include "_cxx17.h"

namespace common_lib {
    namespace utilities {

        static bool _is_big_endian = false;

        struct _endianess_static_init {

            _endianess_static_init() noexcept {
                _is_big_endian = htonl(15) == 15;
            }

        };

        _CXX17_MAYBE_UNUSED
        static _endianess_static_init _init;

        bool machine_is_big_endian() {
            return _is_big_endian;
        }

        Endianess get_machine_endian() {
            return machine_is_big_endian() ? Endianess::BigEndian : Endianess::LittleEndian;
        }

    }
}
