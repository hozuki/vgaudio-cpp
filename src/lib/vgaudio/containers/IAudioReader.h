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

    struct AudioWithConfig;

    using namespace std;
    using namespace vgaudio::formats;
    using namespace common_lib::io;
    using namespace common_lib::utilities;

    struct IAudioReader {

        virtual ~IAudioReader() = default;

        virtual shared_ptr<IAudioFormat> readFormat(const shared_ptr<Stream> &stream) = 0;

        virtual shared_ptr<IAudioFormat> readFormat(const array_ptr<uint8_t> &file);

        virtual shared_ptr<AudioData> read(const shared_ptr<Stream> &stream) = 0;

        virtual shared_ptr<AudioData> read(const array_ptr<uint8_t> &file);

        virtual shared_ptr<AudioWithConfig> readWithConfig(const shared_ptr<Stream> &stream) = 0;

        virtual shared_ptr<AudioWithConfig> readWithConfig(const array_ptr<uint8_t> &file);

    protected:

        IAudioReader() = default;

    };

}
