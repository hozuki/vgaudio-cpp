#pragma once

#include <cstdint>
#include <string>

namespace vgaudio::codecs::criadx {

    struct CriAdxKey {

        CriAdxKey(int32_t seed, int32_t mul, int32_t inc);

        explicit CriAdxKey(uint64_t keyCode);

        explicit CriAdxKey(const std::string &keyString);

    public:

        [[nodiscard]]
        int32_t getSeed() const;

        [[nodiscard]]
        int32_t getMul() const;

        [[nodiscard]]
        int32_t getInc() const;

        [[nodiscard]]
        uint64_t getKeyCode() const;

        [[nodiscard]]
        const std::string &getKeyString() const;

    private:

        int32_t _seed;
        int32_t _mul;
        int32_t _inc;
        std::string _keyString;

    };

}
