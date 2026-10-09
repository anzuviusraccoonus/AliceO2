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
    void sendOutput(framework::ProcessingContext& ctx);

    bool mDebugMode;
    Decoder mDecoder{2}; // TODO: get # of links from a config somewhere
                         //       (also have separate decoders for each subdetector)
};

framework::DataProcessorSpec getRawDecoderSpec(bool debug);

} // namespace o2::focal

#endif
