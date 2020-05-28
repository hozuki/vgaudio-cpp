#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioWriter.h"
#include "HcaConfiguration.h"

namespace common_lib {
    namespace io {
        struct BinaryWriter;
    }
}

namespace vgaudio {
    namespace codecs {
        namespace crihca {
            struct HcaInfo;
        }
    }
}

namespace vgaudio {
    namespace utilities {
        struct Crc16;
    }
}

namespace vgaudio {
    namespace containers {
        namespace hca {

            struct HcaWriter : public AudioWriter<HcaConfiguration> {

                DECLARE_CLASS_TEMPLATE(HcaWriter, AudioWriter, HcaConfiguration);

            private:

                std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> _hca;
                common_lib::utilities::jarray2_ptr<uint8_t> _audioData;
                const uint16_t _version;
                std::shared_ptr<vgaudio::utilities::Crc16> _crc;

            public:

                HcaWriter();

                ~HcaWriter() override = default;

            protected:

                int32_t getFileSize() override;

                void setupWriter(const std::shared_ptr<vgaudio::formats::AudioData> &audio) override;

                void writeStream(const std::shared_ptr<common_lib::io::Stream> &stream) override;

            private:

                [[nodiscard]]
                int32_t getHeaderSize() const;

                void writeHeader(const std::shared_ptr<common_lib::io::Stream> &stream);

                void writeHcaChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeFmtChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeCompChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeLoopChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeCiphChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeRvaChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeCommChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writePadChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeChunkId(const std::shared_ptr<common_lib::io::BinaryWriter> &writer, const char *id, size_t size = 4);

                void writeAudioData(const std::shared_ptr<common_lib::io::Stream> &stream);

            };

        }
    }
}
