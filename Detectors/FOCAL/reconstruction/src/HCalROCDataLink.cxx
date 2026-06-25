#include "FOCALReconstruction/HCalROCDataLink.h"

using namespace o2::focal;

void HCalROCDataLink::setHeader(unsigned int data) {
  mRawWords[HEADER_INDEX] = data;
  mHeader = HCalDAQHeader(data);
}

void HCalROCDataLink::setCommonMode(unsigned int data) {
  mRawWords[CM_INDEX] = data;
  mCommonMode = HCalChannel(data);
}

void HCalROCDataLink::setCalibration(unsigned int data) {
  mRawWords[CALIB_INDEX] = data;
  mCalibration = HCalChannel(data);
}

void HCalROCDataLink::setChannel(unsigned int data, int index) {
  int rawIndex;
  if (index > 17) {
    rawIndex = index + 3;
  } else {
    rawIndex = index + 2;
  }

  mRawWords[rawIndex] = data;
  mChannels[index] = HCalChannel(data);
}

void HCalROCDataLink::setCRC(unsigned int data) {
  mRawWords[CRC_INDEX] = data;
  mCRC = data;
}


HCalDAQHeader HCalROCDataLink::getHeader() {
  return mHeader;
}

HCalChannel HCalROCDataLink::getCommonMode() {
  return mCommonMode;
}

HCalChannel HCalROCDataLink::getCalibration() {
  return mCalibration;
}

HCalChannel HCalROCDataLink::getChannel(int index) {
  return mChannels[index];
}

unsigned int HCalROCDataLink::getCRC() {
  return mCRC;
}

std::array<unsigned int, constants::HCAL_NUM_GBT_LINES_PER_LINK> HCalROCDataLink::getWords() {
  return mRawWords;
}
