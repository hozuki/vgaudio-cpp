#pragma once

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/Guid.h"

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::utilities;

    struct MediaSubtypes {

        IMPLEMENT_STATIC_CLASS(MediaSubtypes);

    public:

        static const Guid Pcm;

        static const Guid Atrac9;

    };

}
