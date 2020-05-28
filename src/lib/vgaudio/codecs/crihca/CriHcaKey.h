#pragma once

#include <cstdint>

#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/traits.h"
#include "CriHcaKeyType.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaKey final {

                DECLARE_ROOT_CLASS(CriHcaKey);

                explicit CriHcaKey(CriHcaKeyType type);

                explicit CriHcaKey(uint64_t keyCode);

                ~CriHcaKey() = default;

            private:

                CriHcaKeyType _keyType;
                uint64_t _keyCode;
                common_lib::utilities::narray_ptr<uint8_t, 256> _decryptionTable;
                common_lib::utilities::narray_ptr<uint8_t, 256> _encryptionTable;

            public:

                [[nodiscard]]
                CriHcaKeyType getKeyType() const;

                uint64_t getKeyCode(uint64_t *pKey = nullptr) const;

                void getKeyCode(uint64_t &key) const;

                void getKeyCode(uint32_t &key1, uint32_t &key2) const;

                [[nodiscard]]
                common_lib::utilities::narray_ptr<uint8_t, 256> getDecryptionTable() const;

                [[nodiscard]]
                common_lib::utilities::narray_ptr<uint8_t, 256> getEncryptionTable() const;

            };

        }
    }
}
