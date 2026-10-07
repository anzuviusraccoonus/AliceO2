#include "DataFormatsFOCAL/EventData.h"

using namespace o2::focal;
using namespace o2::focal::readout;

SampleData::SampleData(int numLinks) {
  for (int i = 0; i < numLinks; ++i) {
    gbtLinks.push_back(GBTLink());
  }
}

EventData::EventData() : mNumGBTLinks(0) {}
EventData::EventData(int numLinks) : mNumGBTLinks(numLinks) {}

void EventData::addSample() {
  samples.emplace_back(mNumGBTLinks);
}
