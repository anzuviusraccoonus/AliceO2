#ifndef ALICEO2_FOCAL_ROOTIFIER_H
#define ALICEO2_FOCAL_ROOTIFIER_H

#include <Rtypes.h>
#include <TFile.h>
#include <TTree.h>

#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"

namespace o2::focal
{

class RootifierSpec : public framework::Task {
  public:
    RootifierSpec() = default;
    ~RootifierSpec() = default;

    void init(framework::InitContext& ctx);
    void run(framework::ProcessingContext& ctx);
    void endOfStream(framework::EndOfStreamContext& ctx);

  private:
    TFile* mFile;
    TTree* mTree;
};

framework::DataProcessorSpec getRootifierSpec();

} // namespace o2::focal

#endif
