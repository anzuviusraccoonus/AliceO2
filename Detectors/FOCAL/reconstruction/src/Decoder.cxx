#include "FOCALReconstruction/Decoder.h"

using namespace o2::focal;
using namespace o2::focal::readout;

Decoder::Decoder(int numLinks) {
  for (int i = 0; i < numLinks; ++i) {
    mGBTLinks.push_back(GBTLink());
    mLinkContexts.push_back(GBTLinkContext());
  }

  reset();
}

void Decoder::reset() {
  for (GBTLink& link : mGBTLinks) {
    link.reset();
  }

  for (GBTLinkContext& ctx : mLinkContexts) {
    ctx.state = GBTLinkContext::State::WaitingForFrame;
    ctx.lines = 0;
    ctx.samples = 0;
  }
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
  
  // Skip the trigger line (no processing needed for now) and padded zeroes
  if (lineType == GBTLineType::Trigger || lineType == GBTLineType::Padding) {
    return;
  }

  const int linkID = line.link_id();
  GBTLink& link = mGBTLinks[linkID];
  GBTLinkContext& ctx = mLinkContexts[linkID];
  switch (ctx.state) {
    case GBTLinkContext::State::WaitingForFrame:
      
      // Pass on idles; daq frame not yet started
      if (lineType == GBTLineType::Idle) {
        break;
      }

      // first line of daq frame
      else if (lineType == GBTLineType::DAQHeader) {
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
        link.fillData(line, ctx.lines++);
        break;
      }

    case GBTLinkContext::State::EndOfFrame:

      // TODO: add functionality to handle more than 1 sample
      // Could also calculate and check CRC checksum here.
      ctx.state = GBTLinkContext::State::Finished;
      [[fallthrough]];

    case GBTLinkContext::State::Finished:
      break;

    case GBTLinkContext::State::Errored:
      break;
  }
}

const std::vector<GBTLink>& Decoder::getGBTLinks() const {
  return mGBTLinks;
}
