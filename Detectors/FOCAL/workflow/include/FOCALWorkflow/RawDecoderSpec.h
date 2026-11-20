#ifndef ALICEO2_FOCAL_RAWDECODERSPEC_H
#define ALICEO2_FOCAL_RAWDEOCDERSPEC_H

#include "FOCALReconstruction/Decoder.h"
#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"

namespace o2::focal
{

class RawDecoderSpec : public framework::Task {
  public:
    RawDecoderSpec() = default;
    RawDecoderSpec(bool debug) : mDebugMode(debug) {}

    ~RawDecoderSpec() = default;

    void init(framework::InitContext& ctx);
    void run(framework::ProcessingContext& ctx);
    void endOfStream(framework::EndOfStreamContext& ctx);

  private:
    void prepareOutput();
    void sendOutput(framework::ProcessingContext& ctx);
    void addEvent(size_t cruID, EventData event);

    bool mDebugMode;

    // Vector of H2GCROC decoders; one for each CRU link
    // Should also be done on a per-detector basis in the future
    std::vector<Decoder> mDecoders;

    // Container for per-CRU event data such that they can be combined later
    // via their interaction records. Currently just for single detector
    std::map<o2::InteractionRecord, CombinedEventData> mPendingEvents;

    // Container for combined event data (for one detector) to send
    std::vector<CombinedEventData> mOutputEvents;
};

framework::DataProcessorSpec getRawDecoderSpec(bool debug);

} // namespace o2::focal

#endif
