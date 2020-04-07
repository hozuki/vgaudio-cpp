#pragma once

#include <cstdint>
#include <memory>

#include "BinaryWriter.h"

namespace common_lib::io {

    struct BinaryWriterBE : BinaryWriter {

        DECLARE_CLASS(BinaryWriterBE, BinaryWriter);

    public:

        explicit BinaryWriterBE(const std::shared_ptr<Stream> &stream);

        ~BinaryWriterBE() override = default;

        int32_t write(uint16_t value) override;

        int32_t write(int16_t value) override;

        int32_t write(uint32_t value) override;

        int32_t write(int32_t value) override;

        int32_t write(uint64_t value) override;

        int32_t write(int64_t value) override;

        int32_t write(float value) override;

        int32_t write(double value) override;

    };

}
