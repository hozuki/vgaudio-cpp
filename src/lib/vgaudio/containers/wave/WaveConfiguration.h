#pragma once

#include "../Configuration.h"
#include "WaveCodec.h"

namespace vgaudio::containers::wave {

    struct WaveConfiguration : public Configuration {

        DECLARE_CLASS(WaveConfiguration, Configuration);

        WaveConfiguration();

        WaveConfiguration(const WaveConfiguration &other) = default;

        explicit WaveConfiguration(const Configuration &other);

        ~WaveConfiguration() override = default;

        WaveCodec codec;

    };

}
