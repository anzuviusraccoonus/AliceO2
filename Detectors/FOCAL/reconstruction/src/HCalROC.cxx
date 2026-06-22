#include "FOCALReconstruction/HCalROC.h"

using namespace o2::focal;

void HCalROC::reset() {
  for (HCalROCDataLink& half : mHalves) {
    half.setHeader(0);
    half.setCommonMode(0);
    half.setCalibration(0);
    half.setCRC(0);

    for (int i = 0; i < 36; ++i) {
      half.setChannel(0, i);
    }
  }
}

HCalROCDataLink HCalROC::getChipHalf(int index) {
  return mHalves[index];
}

void HCalROC::fillHeaderData(HCalDataWord first, HCalDataWord second) {
  mHalves[0].setHeader(first);
  mHalves[1].setHeader(second);
}

void HCalROC::fillCommonModeData(HCalDataWord first, HCalDataWord second) {
  mHalves[0].setCommonMode(first);
  mHalves[1].setCommonMode(second);
}
void HCalROC::fillCalibrationData(HCalDataWord first, HCalDataWord second) {
  mHalves[0].setCalibration(first);
  mHalves[1].setCalibration(second);
}
void HCalROC::fillCRCData(HCalDataWord first, HCalDataWord second) {
  mHalves[0].setCRC(first);
  mHalves[1].setCRC(second);
}
void HCalROC::fillChannelData(HCalDataWord first, HCalDataWord second, int index) {
  mHalves[0].setChannel(first, index);
  mHalves[1].setChannel(second, index);
}
