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
}

void RawDecoderSpec::run(ProcessingContext& ctx) {
  int i = 0;
  std::vector<char> payloadBuffer;

  // If nHBFPerTF=32, we always get only one element here, it seems
  // Not sure what the point of this loop then is
  for (const auto& msgData : framework::InputRecordWalker(ctx.inputs())) {
    const int msgSize = DataRefUtils::getPayloadSize(msgData);
    const o2::header::DataHeader* msgHeader = DataRefUtils::getHeader<o2::header::DataHeader*>(msgData);
    if (mDebugMode) {
      std::cout << i++ << ": " 
                << msgHeader->dataOrigin.str << " / " 
                << msgHeader->dataDescription.str << " / " 
                << msgHeader->subSpecification 
                << " (" << msgSize << ")" 
                << std::endl;
    }

    gsl::span<const char> databuffer(msgData.payload, msgSize);
    int pos = 0;

    // Iterate over all pages in this timeframe - essentially just removes
    // headers and leaves detector payload in payloadBuffer
    while (pos < databuffer.size()) {
      auto      rdh             = reinterpret_cast<const o2::header::RDHAny*>(databuffer.data() + pos);
      const int pageHeaderSize  = o2::raw::RDHUtils::getHeaderSize(rdh);
      const int pagePayloadSize = o2::raw::RDHUtils::getMemorySize(rdh) - pageHeaderSize;
      auto      pagePayload     = databuffer.subspan(pos + pageHeaderSize, pagePayloadSize);

      std::copy(pagePayload.begin(), pagePayload.end(), std::back_inserter(payloadBuffer));
      pos += o2::raw::RDHUtils::getOffsetToNext(rdh);
    }

    mDecoder.decodeBuffer(payloadBuffer);

    if (mDebugMode) {
      std::cout << "Decoded events: " << mDecoder.getEvents().size() << std::endl;
    }
  }
}

void RawDecoderSpec::endOfStream(EndOfStreamContext& ctx) {
}

void RawDecoderSpec::sendOutput(ProcessingContext& ctx) {
  std::vector<EventData> output = mDecoder.getEvents();
  ctx.outputs().snapshot(Output{o2::header::gDataOriginFOC, "EVENTDATA", 0}, 
                         ROOTSerialized<std::vector<EventData>>(output));
}

DataProcessorSpec o2::focal::getRawDecoderSpec(bool debug) {
  constexpr o2::header::DataOrigin origin = o2::header::gDataOriginFOC;

  std::vector<InputSpec> inputs;
  std::vector<OutputSpec> outputs;

  inputs.emplace_back("stf", 
                      ConcreteDataTypeMatcher{origin, o2::header::gDataDescriptionRawData}, 
                      Lifetime::Timeframe);

  return DataProcessorSpec{"FOCALRawDecoderSpec", inputs, outputs,
                           adaptFromTask<RawDecoderSpec>(debug),
                           Options{}};
}
