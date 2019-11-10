#pragma once

#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace common_lib::io {
    struct Stream;
}

namespace vgaudio::formats {
    struct AudioData;
    struct IAudioFormat;
}

namespace vgaudio::containers {

    struct Configuration;

    using namespace std;
    using namespace vgaudio::formats;
    using namespace common_lib::io;
    using namespace common_lib::utilities;

    struct IAudioWriter {

        virtual ~IAudioWriter() = default;

        virtual void writeToStream(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) = 0;

        virtual array_ptr<uint8_t> getFile(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config);

        virtual void writeToStream(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) = 0;

        virtual array_ptr<uint8_t> getFile(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config);

    protected:

        IAudioWriter() = default;

    };

}
