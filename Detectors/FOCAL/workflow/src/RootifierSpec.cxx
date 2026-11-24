#include <iostream>

#include "DataFormatsFOCAL/EventData.h"
#include "DataFormatsFOCAL/Readout.h"
#include "DetectorsRaw/RDHUtils.h"
#include "FOCALWorkflow/RootifierSpec.h"
#include "Framework/DataRefUtils.h"
#include "Framework/InputRecordWalker.h"

#include "DataFormatsFOCAL/ROOTEventRecords.h"

#include <TBranch.h>

using namespace o2::focal;
using namespace o2::framework;

void RootifierSpec::init(InitContext& ctx) {
  mFile = new TFile("focalevents.root", "RECREATE");
  mTree = new TTree("qa", "flattened detector data");
}

void RootifierSpec::run(ProcessingContext& ctx) {
  const auto& events = ctx.inputs().get<std::vector<CombinedEventData>>("hcaldata");

  records::Event eventRecord;

  mTree->Branch("dataLinks", &eventRecord.dataLinks);
  mTree->Branch("channels", &eventRecord.channels);

  for (const auto& event : events) {
    records::flattenEvent(event, eventRecord);
    mTree->Fill();
  }

  mFile->cd();
  mTree->Write();

  //int eventID;
  //int cruID;
  //int sampleID;
  //int gbtID;
  //int rocID;
  //int datalinkID;
  //int channelID;

  //int word;

  //mTree->Branch("eventID", &eventID);
  //mTree->Branch("cruID", &cruID);
  //mTree->Branch("sampleID", &sampleID);
  //mTree->Branch("gbtID", &gbtID);
  //mTree->Branch("rocID", &rocID);
  //mTree->Branch("datalinkID", &datalinkID);
  //mTree->Branch("channelID", &channelID);
  //
  //mTree->Branch("word", &word);

  //for (size_t ev = 0; ev < events.size(); ++ev) {
  //  eventID = ev;
  //  const auto& event = events[ev];

  //  for (size_t c = 0; c < 4; ++c) {
  //    cruID = c;
  //    const auto& cru = event.getCRUData(c);

  //    for (size_t s = 0; s < cru.getNumberOfSamples(); ++s) {
  //      sampleID = s;
  //      const auto& sample = cru.getData(s);

  //      for (size_t g = 0; g < sample.size(); ++g) {
  //        gbtID = g;
  //        const auto& gbt = sample[g];

  //        for (size_t r = 0; r < 3; ++r) {
  //          rocID = r;
  //          const auto& roc = gbt.getROC(r);

  //          for (size_t l = 0; l < 2; ++l) {
  //            datalinkID = l;
  //            const auto& datalink = roc.getDataLink(l);

  //            for (size_t ch = 0; ch < 36; ++ch) {
  //              channelID = ch;
  //              const auto w = datalink.getChannel(ch);
  //              word = w;

  //              mTree->Fill();
  //            }
  //          }
  //        }
  //      }
  //    }
  //  }
  //}

  //mTree->Write();

  //CombinedEventData ev;
  //mTree->Branch("data", &ev, 32000, 99);
  //for (const auto& event : events) {
  //  std::cout << event.getInteractionRecord() << std::endl;
  //  ev = event;
  //  mTree->Fill();
  //}

  //mTree->Write();
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
