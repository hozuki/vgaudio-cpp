#include "../../codecs/crihca/HcaInfo.h"
#include "HcaStructure.h"

using namespace std;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::containers::hca;

HcaStructure::HcaStructure()
    : version(0), headerSize(0),
      reserved1(0), reserved2(0) {
    hca = make_shared<HcaInfo>();
}
