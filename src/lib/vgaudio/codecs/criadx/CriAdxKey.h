#pragma once

#include <cstdint>
#include <string>

#include "../../../common/utilities/_cxx17.h"

namespace vgaudio {
    namespace codecs {
        namespace criadx {

            struct CriAdxKey {

                CriAdxKey(int32_t seed, int32_t mul, int32_t inc);

                explicit CriAdxKey(uint64_t keyCode);

                explicit CriAdxKey(const std::string &keyString);

            public:

                _CXX17_ATTR_NODISCARD
                int32_t getSeed() const;

                _CXX17_ATTR_NODISCARD
                int32_t getMul() const;

                _CXX17_ATTR_NODISCARD
                int32_t getInc() const;

                _CXX17_ATTR_NODISCARD
                uint64_t getKeyCode() const;

                _CXX17_ATTR_NODISCARD
                const std::string &getKeyString() const;

            private:

                int32_t _seed;
                int32_t _mul;
                int32_t _inc;
                std::string _keyString;

            };

        }
    }
}
