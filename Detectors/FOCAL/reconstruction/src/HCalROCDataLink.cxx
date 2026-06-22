#include "FOCALReconstruction/HCalROCDataLink.h"

using namespace o2::focal;

void HCalROCDataLink::setHeader(unsigned int data) {
  mHeader = HCalDAQHeader(data);
}

void HCalROCDataLink::setCommonMode(unsigned int data) {
  mCommonMode = HCalChannel(data);
}

void HCalROCDataLink::setCalibration(unsigned int data) {
  mCalibration = HCalChannel(data);
}

void HCalROCDataLink::setChannel(unsigned int data, int index) {
  mChannels[index] = HCalChannel(data);
}

void HCalROCDataLink::setCRC(unsigned int data) {
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
