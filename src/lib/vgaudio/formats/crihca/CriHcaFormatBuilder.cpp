#include "../../../common/utilities/runtime_jagged_array.h"
#include "../../../common/utilities/macros.h"
#include "../../codecs/crihca/HcaInfo.h"
#include "CriHcaFormat.h"
#include "CriHcaFormatBuilder.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::formats;
using namespace vgaudio::formats::crihca;

CriHcaFormatBuilder::CriHcaFormatBuilder(const jarray2_ptr<uint8_t> &audioData, const shared_ptr<HcaInfo> &hca)
    : _audioData(audioData), _hca(hca) {
    setSampleRate(hca->sampleRate);
    setSampleCount(hca->sampleCount);

    if (hca->looping) {
        setLooping(true);
        setLoopStart(hca->getLoopStartSample());
        setLoopEnd(hca->getLoopEndSample());
    }
}

jarray2_ptr<uint8_t> CriHcaFormatBuilder::getAudioData() const {
    return _audioData;
}

shared_ptr<HcaInfo> CriHcaFormatBuilder::getHca() const {
    return _hca;
}

int32_t CriHcaFormatBuilder::getChannelCount() const {
    const auto hca = getHca();
    return hca != nullptr ? hca->channelCount : 0;
}

shared_ptr<IAudioFormat> CriHcaFormatBuilder::build() {
    auto thiz = shared_ptr<MyClass>(this, DONT_DELETE(MyClass));
    return make_shared<CriHcaFormat>(thiz);
}
