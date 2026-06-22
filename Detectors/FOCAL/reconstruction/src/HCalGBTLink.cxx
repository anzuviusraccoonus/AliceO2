#include "FOCALReconstruction/HCalGBTLink.h"

using namespace o2::focal;

void HCalGBTLink::reset() {
  for (HCalROC& roc : mROCs) {
    roc.reset();
  }
}

HCalROC HCalGBTLink::getROC(int index) {
  return mROCs[index];
}

void HCalGBTLink::fillData(HCalGBTLine line, int lineNumber) {
  switch (lineNumber) {
    case 0:
      mROCs[0].fillHeaderData(line.words[2], line.words[3]);
      mROCs[1].fillHeaderData(line.words[4], line.words[5]);
      break;

    case 1:
      mROCs[0].fillCommonModeData(line.words[2], line.words[3]);
      mROCs[1].fillCommonModeData(line.words[4], line.words[5]);
      break;

    case 20:
      mROCs[0].fillCalibrationData(line.words[2], line.words[3]);
      mROCs[1].fillCalibrationData(line.words[4], line.words[5]);
      break;

    case 39:
      mROCs[0].fillCRCData(line.words[2], line.words[3]);
      mROCs[1].fillCRCData(line.words[4], line.words[5]);
      break;

    default:
      int channelIndex;
      if (lineNumber < 20) {
        channelIndex = lineNumber - 2;
      } else {
        channelIndex = lineNumber - 3;
      }

      mROCs[0].fillChannelData(line.words[2], line.words[3], channelIndex);
      mROCs[1].fillChannelData(line.words[4], line.words[5], channelIndex);
      break;
  }
}
