#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioWriter.h"
#include "HcaConfiguration.h"

namespace common_lib::io {
    struct BinaryWriter;
}

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
}

namespace vgaudio::utilities {
    struct Crc16;
}

namespace vgaudio::containers::hca {

    using namespace std;
    using namespace vgaudio::containers;
    using namespace vgaudio::codecs::crihca;
    using namespace vgaudio::utilities;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct HcaWriter : public AudioWriter<HcaConfiguration> {

        DECLARE_CLASS_TEMPLATE(HcaWriter, AudioWriter, HcaConfiguration);

    private:

        shared_ptr<HcaInfo> _hca;
        jarray2_ptr<uint8_t> _audioData;
        const uint16_t _version;
        shared_ptr<Crc16> _crc;

    public:

        HcaWriter();

        ~HcaWriter() override = default;

    protected:

        int32_t getFileSize() override;

        void setupWriter(const shared_ptr<AudioData> &audio) override;

        void writeStream(const shared_ptr<Stream> &stream) override;

    private:

        [[nodiscard]]
        int32_t getHeaderSize() const;

        void writeHeader(const shared_ptr<Stream> &stream);

        void writeHcaChunk(const shared_ptr<BinaryWriter> &writer);

        void writeFmtChunk(const shared_ptr<BinaryWriter> &writer);

        void writeCompChunk(const shared_ptr<BinaryWriter> &writer);

        void writeLoopChunk(const shared_ptr<BinaryWriter> &writer);

        void writeCiphChunk(const shared_ptr<BinaryWriter> &writer);

        void writeRvaChunk(const shared_ptr<BinaryWriter> &writer);

        void writeCommChunk(const shared_ptr<BinaryWriter> &writer);

        void writePadChunk(const shared_ptr<BinaryWriter> &writer);

        void writeChunkId(const shared_ptr<BinaryWriter> &writer, const char *id, size_t size = 4);

        void writeAudioData(const shared_ptr<Stream> &stream);

    };

}
