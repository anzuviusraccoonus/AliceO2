#include "DataFormatsFOCAL/EventData.h"

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

const o2::InteractionRecord& EventData::getInteractionRecord() const {
  return mInteractionRecord;
}

CombinedEventData::CombinedEventData(size_t numCRULinks)
  : mCRUData(numCRULinks) {
}

void CombinedEventData::addCRUData(size_t cruID, EventData event) {
  if (mCRUData.at(cruID).getNumberOfSamples() > 0) {
    throw std::runtime_error("Duplicate CRU event!");
  }

  mCRUData[cruID] = std::move(event);
}

void CombinedEventData::setInteractionRecord(o2::InteractionRecord ir) {
  mInteractionRecord = ir;
}

const o2::InteractionRecord& CombinedEventData::getInteractionRecord() const {
  return mInteractionRecord;
}

const EventData& CombinedEventData::getCRUData(size_t cruID) {
  return mCRUData.at(cruID);
}
