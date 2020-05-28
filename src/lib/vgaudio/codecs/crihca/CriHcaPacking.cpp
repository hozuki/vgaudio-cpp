#include <algorithm>

#include "../../../common/utilities/IntHelper.h"
#include "CriHcaTables.h"
#include "CriHcaConstants.h"
#include "CriHcaChannel.h"
#include "CriHcaFrame.h"
#include "HcaInfo.h"
#include "../../utilities/BitWriter.h"
#include "../../utilities/BitReader.h"
#include "../../utilities/Crc16.h"
#include "private/CriHcaPackingStatic.h"
#include "CriHcaPacking.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::utilities;

using C = CriHcaConstants;

int32_t CriHcaPacking::calculateResolution(int32_t scaleFactor, int32_t noiseLevel) {
    if (scaleFactor == 0) {
        return 0;
    }

    int32_t curvePosition = noiseLevel - 5 * scaleFactor / 2 + 2;
    curvePosition = IntHelper::clamp(curvePosition, 0, 58);

    const auto &table = CriHcaTables::getScaleToResolutionCurve();

    return (*table)[curvePosition];
}

bool CriHcaPacking::unpackFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader) {
    if (!UnpackFrameHeader(frame, reader)) {
        return false;
    }

    ReadSpectralCoefficients(frame, reader);

    const auto r = UnpackingWasSuccessful(frame, reader);

    return r;
}

void CriHcaPacking::packFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<Crc16> &crc, const array_ptr<uint8_t> &outBuffer) {
    auto writer = make_shared<BitWriter>(outBuffer);

    writer->write(0xffff, 16);
    writer->write(frame->getAcceptableNoiseLevel(), 9);
    writer->write(frame->getEvaluationBoundary(), 7);

    const auto &hca = frame->getHca();

    for (const auto &channel : *frame->getChannels()) {
        WriteScaleFactors(writer, channel);

        if (channel->type == ChannelType::StereoSecondary) {
            for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                writer->write((*channel->intensity)[sf], 4);
            }
        } else {
            const auto groupCount = hca->hfrGroupCount;

            if (groupCount > 0) {
                for (auto group = 0; group < groupCount; group += 1) {
                    writer->write((*channel->hfrScales)[group], 6);
                }
            }
        }
    }

    for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
        for (const auto &channel : *frame->getChannels()) {
            WriteSpectra(writer, channel, sf);
        }
    }

    writer->alignPosition(8);

    const auto &writerBuffer = writer->getBuffer();
    for (auto i = writer->getPosition() / 8; i < hca->frameSize - 2; i += 1) {
        (*writerBuffer)[i] = 0;
    }

    WriteChecksum(writer, crc, outBuffer);
}
