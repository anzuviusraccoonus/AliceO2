#include <iostream>

#include "DataFormatsFOCAL/EventData.h"
#include "DataFormatsFOCAL/Readout.h"
#include "DetectorsRaw/RDHUtils.h"
#include "FOCALWorkflow/RawDecoderSpec.h"
#include "Framework/DataRefUtils.h"
#include "Framework/InputRecordWalker.h"

using namespace o2::focal;
using namespace o2::framework;

void RawDecoderSpec::init(InitContext& ctx) {
  // TODO: un-hardcode # of CRUs and GBT links
  for (int i = 0; i < 4; ++i) {
    mDecoders.push_back(Decoder{2});
  }
}

void RawDecoderSpec::run(ProcessingContext& ctx) {
  size_t nMessages = 0;

  // Resetting decoders removes any old events parsed
  // Might not be the right thing to do? (if events cross timeframes)
  for (Decoder& decoder : mDecoders) {
    decoder.reset();
  }

  // Loop over incoming messages
  for (const auto& msgData : framework::InputRecordWalker(ctx.inputs())) {
    ++nMessages;
    
    const int msgSize = DataRefUtils::getPayloadSize(msgData);
    const o2::header::DataHeader* msgHeader = DataRefUtils::getHeader<o2::header::DataHeader*>(msgData);

    if (mDebugMode) {
      std::cout << "Message " << nMessages
                << " | TF: " << msgHeader->tfCounter
                << " | 1st orbit: " << msgHeader->firstTForbit
                << " | Origin: " << msgHeader->dataOrigin.str
                << " | Subspec: " << msgHeader->subSpecification
                << " | Desc: " << msgHeader->dataDescription.str
                << " | Size: " << msgSize
                << std::endl;
    }

    gsl::span<const char> databuffer(msgData.payload, msgSize);
    int pos = 0;

    // Iterate over all pages in this message - essentially just removes
    // headers and leaves detector payload in payloadBuffer
    while (pos < databuffer.size()) {
      std::vector<char> payloadBuffer;

      auto      rdh             = reinterpret_cast<const o2::header::RDHAny*>(databuffer.data() + pos);
      auto      ir              = o2::raw::RDHUtils::getTriggerIR(rdh);
      const int pageHeaderSize  = o2::raw::RDHUtils::getHeaderSize(rdh);
      const int pagePayloadSize = o2::raw::RDHUtils::getMemorySize(rdh) - pageHeaderSize;
      auto      pagePayload     = databuffer.subspan(pos + pageHeaderSize, pagePayloadSize);

      const int pageFEEID       = o2::raw::RDHUtils::getFEEID(rdh);
      const int pageLinkID      = o2::raw::RDHUtils::getLinkID(rdh);
      const int pageCRUID       = o2::raw::RDHUtils::getCRUID(rdh);
      const int pageEPID        = o2::raw::RDHUtils::getEndPointID(rdh);
      const int pageCounter     = o2::raw::RDHUtils::getPageCounter(rdh);
      const int pageOrbit       = o2::raw::RDHUtils::getTriggerOrbit(rdh);
      const int pagePacketCntr  = o2::raw::RDHUtils::getPacketCounter(rdh);
      const int pageTrigger     = o2::raw::RDHUtils::getTriggerType(rdh);
      const int pageStopBit     = o2::raw::RDHUtils::getStop(rdh);

      if (mDebugMode) {
        std::cout << "  Page " << pageCounter
                  << " | IR: " << ir
                  << " | CRU: " << pageCRUID
                  << " | FEE: " << pageFEEID
                  << " | Payload size: " << pagePayloadSize
                  << " | Packet counter: " << pagePacketCntr
                  << " | Trigger: " << pageTrigger
                  << " | Stop bit: " << pageStopBit
                  << std::endl;
      }

      std::copy(pagePayload.begin(), pagePayload.end(), std::back_inserter(payloadBuffer));
      pos += o2::raw::RDHUtils::getOffsetToNext(rdh);

      mDecoders[pageCRUID].decodeBuffer(payloadBuffer);
    }
  }

  int d = 0;
  for (const Decoder& decoder : mDecoders) {
    const std::vector<EventData>& events = decoder.getEvents();
    for (const EventData& event : events) {
      addEvent(d, event);
    }

    ++d;
  }

  prepareOutput();
  sendOutput(ctx);
}

void RawDecoderSpec::endOfStream(EndOfStreamContext& ctx) {
}

void RawDecoderSpec::sendOutput(ProcessingContext& ctx) {
  ctx.outputs().snapshot(Output{o2::header::gDataOriginFOC, "EVENTDATA", 0},
                         mOutputEvents);

  mOutputEvents.clear();
}

void RawDecoderSpec::addEvent(size_t cruID, EventData event) {
  const auto ir = event.getInteractionRecord();
  auto [it, inserted] = mPendingEvents.try_emplace(ir, 4);
  auto& combinedEvent = it->second;
  if (inserted) {
    combinedEvent.setInteractionRecord(ir);
  }

  if (combinedEvent.getCRUData(cruID).getNumberOfSamples() > 0) {
    std::cout << "WARNING: duplicate event from CRU " << cruID
              << " for IR " << ir
              << std::endl;
    return;
  }

  combinedEvent.addCRUData(cruID, std::move(event));
}

void RawDecoderSpec::prepareOutput() {
  mOutputEvents.clear();
  for (auto& [ir, event] : mPendingEvents) {
    mOutputEvents.push_back(std::move(event));
  }
  
  mPendingEvents.clear();
}

DataProcessorSpec o2::focal::getRawDecoderSpec(bool debug) {
  constexpr o2::header::DataOrigin origin = o2::header::gDataOriginFOC;

  std::vector<InputSpec> inputs;
  std::vector<OutputSpec> outputs;

  inputs.emplace_back("stf", 
                      ConcreteDataTypeMatcher{origin, o2::header::gDataDescriptionRawData}, 
                      Lifetime::Timeframe);

  outputs.emplace_back(origin, "EVENTDATA", 0,
                       Lifetime::Timeframe);

  return DataProcessorSpec{"FOCALRawDecoderSpec", inputs, outputs,
                           adaptFromTask<RawDecoderSpec>(debug),
                           Options{}};
}
