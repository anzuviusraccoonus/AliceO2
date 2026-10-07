#include <iostream>
#include <iomanip>

#include "FOCALReconstruction/Decoder.h"

using namespace o2::focal;
using namespace o2::focal::readout;

Decoder::Decoder(int numLinks) : mNumGBTLinks(numLinks) {
  for (int i = 0; i < numLinks; ++i) {
    mLinkContexts.push_back(GBTLinkContext());
  }

  reset();
}

void Decoder::reset() {
  mEvents.clear();
  mCurrentEvent = nullptr;
}

void Decoder::prepareNewEvent() {
  for (GBTLinkContext& ctx : mLinkContexts) {
    ctx.state = GBTLinkContext::State::WaitingForFrame;
    ctx.lines = 0;
    ctx.samples = 0;
  }

  mEvents.emplace_back(mNumGBTLinks);
  mCurrentEvent = &(mEvents.back());
}

void Decoder::decodeBuffer(std::span<const char> buffer) {
  if (buffer.size() == 0) {
    return;
  }

  // Cast the buffer to a vector of "lines" so we can easily iterate over them
  std::span<const GBTLine> lines(reinterpret_cast<const GBTLine*>(buffer.data()), 
                                 buffer.size() / sizeof(GBTLine));

  for (const GBTLine& line : lines) {
    processLine(line);
  }
}

void Decoder::processLine(const GBTLine& line) {
  GBTLineType lineType = classify(line); 
  if (lineType == GBTLineType::Padding) { return; }
  if (lineType == GBTLineType::Trigger) {
    prepareNewEvent();
    return;
  }

  const int linkID = line.link_id();
  GBTLinkContext& ctx = mLinkContexts[linkID];
  switch (ctx.state) {
    case GBTLinkContext::State::WaitingForFrame:
      
      // Pass on idles; daq frame not yet started
      if (lineType == GBTLineType::Idle) {
        break;
      }

      // first line of daq frame
      else if (lineType == GBTLineType::DAQHeader) {

        // if this is a new sample, add it to the current event data
        if (mCurrentEvent->samples.size() == ctx.samples) {
          mCurrentEvent->addSample();
        }

        ctx.state = GBTLinkContext::State::ReadingFrame;
        [[fallthrough]];
      }

      // (not idle) & (not daqh) & (not trigger) & (not ==0)
      else {
        ctx.state = GBTLinkContext::State::Errored;
        break;
      }

    case GBTLinkContext::State::ReadingFrame:

      // Idle word indicates end-of-frame
      // Should happen after 40 lines/words read
      if (lineType == GBTLineType::Idle) {
        ctx.state = GBTLinkContext::State::EndOfFrame;
        [[fallthrough]];
      }

      // Anything else is considered data to be parsed
      else {
        mCurrentEvent->samples[ctx.samples].gbtLinks[linkID].fillData(line, ctx.lines++);
        break;
      }

    case GBTLinkContext::State::EndOfFrame:

      // TODO: Could also calculate and check CRC checksum here.
      ++ctx.samples;
      ctx.lines = 0;
      ctx.state = GBTLinkContext::State::WaitingForFrame;
      break;

    case GBTLinkContext::State::Finished:
      break;

    case GBTLinkContext::State::Errored:
      break;
  }
}

const EventData& Decoder::getEvent() const {
  return mEvents.back();
}

const EventData& Decoder::getEvent(int index) const {
  return mEvents[index];
}

const std::vector<EventData>& Decoder::getEvents() const {
  return mEvents;
}
