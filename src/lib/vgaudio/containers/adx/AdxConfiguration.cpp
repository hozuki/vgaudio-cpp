#include "AdxConfiguration.h"

using namespace vgaudio::containers::adx;
using namespace vgaudio::codecs::criadx;

AdxConfiguration::AdxConfiguration()
    : version(4), encryptionType(0), frameSize(18), filter(2), type(CriAdxType::Linear) {
}

AdxConfiguration::AdxConfiguration(const Configuration &other)
    : MyBase(other),
      version(4), encryptionType(0), frameSize(18), filter(2), type(CriAdxType::Linear) {
}
