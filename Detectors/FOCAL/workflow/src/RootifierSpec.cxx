#include <TBranch.h>

#include "DetectorsRaw/RDHUtils.h"
#include "Framework/DataRefUtils.h"
#include "Framework/InputRecordWalker.h"

#include "DataFormatsFOCAL/EventData.h"
#include "DataFormatsFOCAL/Readout.h"
#include "DataFormatsFOCAL/ROOTEventRecords.h"
#include "FOCALWorkflow/RootifierSpec.h"

using namespace o2::focal;
using namespace o2::framework;

void RootifierSpec::init(InitContext& ctx) {
  mFile = new TFile("focalevents.root", "RECREATE");
  mTree = new TTree("qa", "description goes here");
}

void RootifierSpec::run(ProcessingContext& ctx) {
  const auto& events = ctx.inputs().get<std::vector<CombinedEventData>>("hcaldata");

  records::Event eventRecord;

  mTree->Branch("datalinks", &eventRecord.dataLinks);
  mTree->Branch("channels", &eventRecord.channels);

  for (const auto& event : events) {
    records::flattenEvent(event, eventRecord);
    mTree->Fill();
  }

  mTree->Write();
}

void RootifierSpec::endOfStream(EndOfStreamContext& ctx) {
  mFile->Close();
}

DataProcessorSpec o2::focal::getRootifierSpec() {
  constexpr o2::header::DataOrigin origin = o2::header::gDataOriginFOC;

  std::vector<InputSpec> inputs;
  std::vector<OutputSpec> outputs;

  inputs.emplace_back("hcaldata", 
                      ConcreteDataTypeMatcher{origin, "EVENTDATA"});
  
  return DataProcessorSpec{"FOCALRootifierSpec", inputs, outputs,
                           adaptFromTask<RootifierSpec>(),
                           Options{}};
}
