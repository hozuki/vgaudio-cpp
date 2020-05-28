#pragma once

#include <cstdint>
#include <string>
#include <iostream>

namespace common_lib {
    namespace utilities {

        struct Guid final {

        public:

            uint32_t data1;
            uint16_t data2;
            uint16_t data3;
            uint8_t data4[8];

            Guid() noexcept;

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint64_t d4) noexcept;

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, const uint8_t d5[6]);

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, const uint8_t d4[8]);

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint16_t d5l, uint32_t d5h) noexcept;

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint32_t d5l, uint16_t d5h) noexcept;

            Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint64_t d5) noexcept;

            Guid(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4) noexcept;

            explicit Guid(const uint8_t data[16]);

            Guid(const Guid &other) = default;

            Guid(Guid &&other) noexcept = default;

            Guid &operator=(const Guid &other) = default;

            Guid &operator=(Guid &&other) noexcept = default;

            bool operator==(const Guid &other);

            bool operator!=(const Guid &other);

            [[nodiscard]]
            std::size_t getHashCode() const;

            [[nodiscard]]
            std::string toString() const;

        };

        std::ostream &operator<<(std::ostream &stream, const Guid &guid);

    }
}

namespace std {

    template<typename T>
    struct hash;

    template<>
    struct hash<::common_lib::utilities::Guid> {
        typedef ::common_lib::utilities::Guid argument_type;
        typedef std::size_t result_type;

        result_type operator()(argument_type const &o) const {
            return o.getHashCode();
        }
    };

}
