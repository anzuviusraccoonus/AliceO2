#include <stdexcept>

#include "DataFormatsFOCAL/ChannelData.h"


using namespace o2::focal;

ChannelData::ChannelData(DAQDataWord word, H2GCROCMode mode)
                        : mWord(word), mMode(mode) {}

DAQDataWord ChannelData::raw() const {
  return mWord;
}

H2GCROCMode ChannelData::mode() const {
  return mMode;
}

ChannelDataType ChannelData::type() const {
  if (mMode == H2GCROCMode::Characterization) {
    return ChannelDataType::Characterization;
  }

  else if (!tc()) {
    return ChannelDataType::ADC;
  }

  else if (tc()) {
    return ChannelDataType::TOT;
  }

  else {
    return ChannelDataType::Undefined;
  }
}

bool ChannelData::tc() const {
  return mWord.bit31();
}

bool ChannelData::tp() const {
  return mWord.bit30();
}

bool ChannelData::hasADC() const {
  return (type() == ChannelDataType::Characterization ||
          type() == ChannelDataType::ADC);
}

bool ChannelData::hasTOT() const {
  return (type() == ChannelDataType::Characterization ||
          type() == ChannelDataType::TOT);
}

bool ChannelData::hasTOA() const {
  return true; // TOA is always available
}

bool ChannelData::hasPrevADC() const {
  return mode() == H2GCROCMode::Normal;
}

uint16_t ChannelData::adc() const {
  if (!hasADC()) {
    throw std::logic_error("ADC measurement is not available for this channel");
  }

  if (mode() == H2GCROCMode::Characterization) {
    return mWord.field1();
  }

  return mWord.field2();
}

uint16_t ChannelData::prevAdc() const {
  if (!hasPrevADC()) {
    throw std::logic_error("Previous bunch crossing ADC measurement is not available for this channel");
  }

  return mWord.field1();
}

uint16_t ChannelData::tot() const {
  if (!hasTOT()) {
    throw std::logic_error("TOT measurement is not available for this channel");
  }

  return mWord.field2();
}

uint16_t ChannelData::toa() const {
  return mWord.field3();
}





























