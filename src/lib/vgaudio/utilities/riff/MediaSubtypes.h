#pragma once

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/Guid.h"

namespace vgaudio::utilities::riff {

    struct MediaSubtypes {

        IMPLEMENT_STATIC_CLASS(MediaSubtypes);

    public:

        static const common_lib::utilities::Guid Pcm;

        static const common_lib::utilities::Guid Atrac9;

    };

}
