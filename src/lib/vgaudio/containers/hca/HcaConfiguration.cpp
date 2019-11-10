#include "HcaConfiguration.h"

using namespace vgaudio::codecs::crihca;
using namespace vgaudio::containers::hca;

HcaConfiguration::HcaConfiguration()
    : quality(CriHcaQuality::NotSet), bitrate(0), limitBitrate(false) {
}

HcaConfiguration::HcaConfiguration(const Configuration &other)
    : MyBase(other), quality(CriHcaQuality::NotSet), bitrate(0), limitBitrate(false) {
}
