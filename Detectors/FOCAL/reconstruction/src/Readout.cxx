#include <ranges>

#include "FOCALReconstruction/Readout.h"

using namespace o2::focal;
using namespace o2::focal::readout;

void DataLink::setHeader(uint32_t data) {
  mHeader = DAQHeaderWord(data);
}

void DataLink::setCommonMode(uint32_t data) {
  mCommonMode = DAQDataWord(data);
}

void DataLink::setCalibration(uint32_t data) {
  mCalibration = DAQDataWord(data);
}

void DataLink::setCRC(uint32_t data) {
  mCRC = DAQDataWord(data);
}

void DataLink::setChannel(uint32_t data, size_t index) {
  mChannels[index] = DAQDataWord(data);
}

DAQHeaderWord DataLink::getHeader() const {
  return mHeader;
}

DAQDataWord DataLink::getCommonMode() const {
  return mCommonMode;
}

DAQDataWord DataLink::getCalibration() const {
  return mCalibration;
}

DAQDataWord DataLink::getCRC() const {
  return mCRC;
}

DAQDataWord DataLink::getChannel(size_t index) const {
  return mChannels[index];
}


void ReadoutChip::reset() {
  mDataLinks = {};
}

void ReadoutChip::fillHeaderData(uint32_t data, size_t datalink) {
  mDataLinks[datalink].setHeader(data);
}

void ReadoutChip::fillCommonModeData(uint32_t data, size_t datalink) {
  mDataLinks[datalink].setCommonMode(data);
}

void ReadoutChip::fillCalibrationData(uint32_t data, size_t datalink) {
  mDataLinks[datalink].setCalibration(data);
}

void ReadoutChip::fillCRCData(uint32_t data, size_t datalink) {
  mDataLinks[datalink].setCRC(data);
}

void ReadoutChip::fillChannelData(uint32_t data, size_t datalink, size_t index) {
  mDataLinks[datalink].setChannel(data, index);
}

std::span<DataLink> ReadoutChip::getDataLinks() {
  return mDataLinks;
}


void GBTLink::reset() {
  mReadoutChips = {};
}

auto GBTLink::getDataLinks() {
  return mReadoutChips | 
         std::views::transform([](ReadoutChip& roc) { return roc.getDataLinks(); }) | 
         std::views::join;
}

void GBTLink::fillData(const GBTLine& line, size_t lineNumber) {
  size_t i = 2;
  for (DataLink& dataLink : getDataLinks()) {
    switch (lineNumber) {
      case 0:
        dataLink.setHeader(line.words[i]);
        break;

      case 1:
        dataLink.setCommonMode(line.words[i]);
        break;

      case 20:
        dataLink.setCalibration(line.words[i]);
        break;

      case 39:
        dataLink.setCRC(line.words[i]);
        break;

      default:

        // Words 0, 1, 20 and 39 are not real channels; the rest are -
        // this is the conversion from word # to readout channel index
        size_t channelIndex = (lineNumber < 20) ? (lineNumber - 2) : (lineNumber - 3);

        dataLink.setChannel(line.words[i], channelIndex);
        break;
    }

    ++i;
  }
}









