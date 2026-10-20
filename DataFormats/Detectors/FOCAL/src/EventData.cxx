#include "DataFormatsFOCAL/EventData.h"
#include <iostream> // remove this
using namespace o2::focal;
using namespace o2::focal::readout;

SampleData::SampleData(int numLinks)
  : gbtLinks(numLinks) {}

EventData::EventData(int numLinks) 
  : mNumGBTLinks(numLinks) {}

void EventData::addSample() {
  mSamples.emplace_back(mNumGBTLinks);
}

std::vector<readout::GBTLink>& EventData::getData(size_t sampleIndex) {
  return mSamples.at(sampleIndex).gbtLinks;
}

const std::vector<readout::GBTLink>& EventData::getData(size_t sampleIndex) const {
  return mSamples.at(sampleIndex).gbtLinks;
}

size_t EventData::getNumberOfSamples() const {
  return mSamples.size();
}

void EventData::setInteractionRecord(o2::InteractionRecord ir) {
  mInteractionRecord = ir;
}

void EventData::printInteractionRecord() const {
  std::cout << mInteractionRecord << std::endl;
}
