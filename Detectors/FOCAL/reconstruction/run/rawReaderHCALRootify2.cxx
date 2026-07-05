// // Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// // See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// // All rights not expressly granted are reserved.
// //
// // This software is distributed under the terms of the GNU General Public
// // License v3 (GPL Version 3), copied verbatim in the file "COPYING".
// //
// // In applying this license CERN does not waive the privileges and immunities
// // granted to it by virtue of its status as an Intergovernmental Organization
// // or submit itself to any jurisdiction.

// /// \file rawReaderFileNew.cxx
// /// \author Markus Fasel <markus.fasel@cern.ch>, Oak Ridge National Laboratory

// #include <bitset>
// #include <iostream>
// #include <boost/program_options.hpp>
// #include <gsl/span>
// #include <fairlogger/Logger.h>

// #include <TFile.h>
// #include <TTree.h>


// #include "CommonConstants/Triggers.h"
// #include "DetectorsRaw/RawFileReader.h"
// #include "DetectorsRaw/RDHUtils.h"
// #include "FOCALReconstruction/HCalDataWord.h"
// #include "FOCALReconstruction/HCalGBTLink.h"
// #include "FOCALReconstruction/HCalDecoder.h"
// #include "Headers/RDHAny.h"
// #include "DataFormatsFOCAL/Constants.h"

// namespace bpo = boost::program_options;

// // Data for tree
// struct HCALTreeData {
//   static constexpr int NSAMPLES  = o2::focal::constants::HCAL_NUM_SAMPLES_PER_EVENT; // 16
//   static constexpr int NLINKS    = o2::focal::constants::HCAL_NUM_GBT_LINKS;         // 2
//   static constexpr int NROCS     = o2::focal::constants::HCAL_NUM_ROCS_PER_LINK;     // 2
//   static constexpr int NHALVES   = 2;
//   static constexpr int NCHANNELS = o2::focal::constants::HCAL_NUM_CHANNELS_PER_ROC_HALF; // 36

//   int mBCid;
//   int mOrbit;
//   int mADC[NSAMPLES][NLINKS][NROCS][NHALVES][NCHANNELS];
//   int mTOA[NSAMPLES][NLINKS][NROCS][NHALVES][NCHANNELS];
//   int mTOT[NSAMPLES][NLINKS][NROCS][NHALVES][NCHANNELS];


//   TTree* mTree = nullptr;

//   void connectTree(TTree* hcaltree)
//   {
//     mTree = hcaltree;
//     mTree->Branch("ORBIT", &mOrbit, "ORBIT/I");
//     mTree->Branch("BCID", &mBCid, "BCID/I");
//     mTree->Branch("ADC", &mADC, "ADC[16][2][2][2][36]/I");
//     mTree->Branch("TOA", &mTOA, "TOA[16][2][2][2][36]/I");
//     mTree->Branch("TOT", &mTOT, "TOT[16][2][2][2][36]/I");

//   }

//   void reset()
//   {
//     mBCid = 0;
//     mOrbit = 0;
//     memset(mADC, 0, sizeof(mADC));
//     memset(mTOA, 0, sizeof(mTOA));
//     memset(mTOT, 0, sizeof(mTOT));
//   }

//   void setInteractionRecord(const o2::InteractionRecord& ir)
//   {
//     mBCid = ir.bc;
//     mOrbit = ir.orbit;
//   }

//   void fill(const std::array<std::array<o2::focal::HCalGBTLink, NLINKS>, NSAMPLES>& links)
//   {
//     for (int sample = 0; sample < NSAMPLES; ++sample) {
//       for (int link_id = 0; link_id < NLINKS; ++link_id) {
//         auto currentLink = links[sample][link_id];
//         for (int roc_id = 0; roc_id < NROCS; ++roc_id) {
//           auto currentROC = currentLink.getROC(roc_id);
//           for (int half = 0; half < NHALVES; ++half) {
//             auto currentHalf = currentROC.getChipHalf(half);
//             for (int chn = 0; chn < NCHANNELS; ++chn) {
//               auto ch = currentHalf.getChannel(chn);
//               mADC[sample][link_id][roc_id][half][chn] = ch.adc;
//               mTOT[sample][link_id][roc_id][half][chn] = ch.tot;
//               mTOA[sample][link_id][roc_id][half][chn] = ch.toa;
//             }
//           }
//         }
//       }
//     }
//   }

//   void fillTree()
//   {
//     mTree->Fill();
//   }
// };

// int convertHCALData(gsl::span<const char> hcalrawdata, const o2::InteractionRecord& currentir, HCALTreeData& rootified)
// {
//     o2::focal::HCalDecoder decoder;

//   decoder.reset();
//   decoder.decodeBuffer(hcalrawdata);   
//   if (!decoder.hasEventData()) { return 0; }

//   auto links = decoder.getData();  
//   rootified.reset();
//   rootified.setInteractionRecord(currentir);
//   rootified.fill(links);
//   rootified.fillTree();
//   return 1;

// }

// int main(int argc, char** argv)
// {
//   bpo::variables_map vm;
//   bpo::options_description opt_general("Usage:\n  " + std::string(argv[0]) +
//                                        " <cmds/options>\n"
//                                        "  Tool will decode the DDLx data 0\n"
//                                        "Commands / Options");
//   bpo::options_description opt_hidden("");
//   bpo::options_description opt_all;
//   bpo::positional_options_description opt_pos;

//   try {
//     auto add_option = opt_general.add_options();
//     add_option("help,h", "Print this help message");
//     add_option("verbose,v", bpo::value<uint32_t>()->default_value(0), "Select verbosity level [0 = no output]");
//     add_option("version", "Print version information");
//     add_option("input-file,i", bpo::value<std::string>()->required(), "Specifies input file. Multiple files can be parsed separated by ,");
//     add_option("output-file,o", bpo::value<std::string>()->default_value("FOCALHCALData.root"), "Output file for rootified data");
//     add_option("readout,r", bpo::value<std::string>()->default_value("RORC"), "Readout mode (RORC or CRU)");
//     add_option("debug,d", bpo::value<uint32_t>()->default_value(0), "Select debug output level [0 = no debug output]");

//     opt_all.add(opt_general).add(opt_hidden);
//     bpo::store(bpo::command_line_parser(argc, argv).options(opt_all).positional(opt_pos).run(), vm);

//     if (vm.count("help") || argc == 1) {
//       std::cout << opt_general << std::endl;
//       exit(0);
//     }

//     if (vm.count("version")) {
//       // std::cout << GitInfo();
//       exit(0);
//     }

//     bpo::notify(vm);
//   } catch (bpo::error& e) {
//     std::cerr << "ERROR: " << e.what() << std::endl
//               << std::endl;
//     std::cerr << opt_general << std::endl;
//     exit(1);
//   } catch (std::exception& e) {
//     std::cerr << e.what() << ", application will now exit" << std::endl;
//     exit(2);
//   }

//   auto rawfilename = vm["input-file"].as<std::string>();
//   auto rootfilename = vm["output-file"].as<std::string>();
//   auto readoutmode = vm["readout"].as<std::string>();

//   std::vector<std::string> inputfiles;
//   if (rawfilename.find(",") != std::string::npos) {
//     // Multiple input file mode
//     std::stringstream parser(rawfilename);
//     std::string buffer;
//     while (std::getline(parser, buffer, ',')) {
//       LOG(info) << "Adding " << buffer;
//       inputfiles.push_back(buffer);
//     }
//     LOG(info) << "Found " << inputfiles.size() << " input files to process";
//   } else {
//     // Processing just a single input file
//     LOG(info) << "Adding " << rawfilename;
//     inputfiles.push_back(rawfilename);
//   }

//   o2::raw::RawFileReader::ReadoutCardType readout = o2::raw::RawFileReader::RORC;
//   if (readoutmode != "RORC" && readoutmode != "CRU") {
//     LOG(error) << "Unknown readout mode - select RORC or CRU";
//     exit(3);
//   } else if (readoutmode == "RORC") {
//     LOG(info) << "Reconstructing in C-RORC mode";
//     readout = o2::raw::RawFileReader::RORC;
//   } else {
//     LOG(info) << "Reconstructing in CRU mode";
//     readout = o2::raw::RawFileReader::CRU;
//   }

//   const bool isCfgFile = rawfilename.size() > 4 &&
//                          rawfilename.compare(rawfilename.size() - 4, 4, ".cfg") == 0;
//   o2::raw::RawFileReader reader(isCfgFile ? rawfilename : "");
//   reader.setDefaultDataOrigin(o2::header::gDataOriginFOC);
//   reader.setDefaultDataDescription(o2::header::gDataDescriptionRawData);
//   reader.setDefaultReadoutCardType(readout);
//   if (!isCfgFile) {
//     for (auto rawfile : inputfiles) {
//       LOG(debug) << "Adding " << rawfile << " to raw reader";
//       reader.addFile(rawfile);
//     }
//   } else {
//     LOG(info) << "Reading file list from config: " << rawfilename;
//   }
//   reader.init();

//   std::unique_ptr<TFile> rootfilewriter(TFile::Open(rootfilename.data(), "RECREATE"));
//   rootfilewriter->cd();
//   TTree* hcaltree = new TTree("HCALData", "HCALData");
//   hcaltree->SetAutoSave(0); //added to avoid partial duplicates being saved in the root tree, uncomment to change this for backup purposes
//   HCALTreeData rootified;
//   rootified.connectTree(hcaltree);

//   int nHBFprocessed = 0, nTFprocessed = 0, nEventsProcessed = 0;
//   std::map<int, int> nEvnetsHBF;
//   while (1) {
//     int tfID = reader.getNextTFToRead();
//     if (tfID >= reader.getNTimeFrames()) {
//       LOG(info) << "nothing left to read after " << tfID << " TFs read";
//       break;
//     }
//     std::vector<char> rawtf; // where to put extracted data
//     for (int il = 0; il < reader.getNLinks(); il++) {
//       auto& link = reader.getLink(il);

//       auto sz = link.getNextTFSize(); // size in bytes needed for the next TF of this link
//       rawtf.resize(sz);
//       link.readNextTF(rawtf.data());
//       gsl::span<char> dataBuffer(rawtf);

//       // Parse
//       std::vector<char> hbfbuffer;
//       int currentpos = 0;
//       o2::InteractionRecord currentir;
//       while (currentpos < dataBuffer.size()) {
//         auto rdh = reinterpret_cast<const o2::header::RDHAny*>(dataBuffer.data() + currentpos);
//         o2::raw::RDHUtils::printRDH(rdh);
//         if (o2::raw::RDHUtils::getMemorySize(rdh) == o2::raw::RDHUtils::getHeaderSize(rdh)) {
//           auto trigger = o2::raw::RDHUtils::getTriggerType(rdh);
//           if (trigger & o2::trigger::SOT || trigger & o2::trigger::HB) {
//             if (o2::raw::RDHUtils::getStop(rdh)) {
//               LOG(debug) << "Stop bit received - processing payload";
//               auto nevents = convertHCALData(hbfbuffer, currentir, rootified);
//               hbfbuffer.clear();
//               nHBFprocessed++;
//               nEventsProcessed += nevents;
//               auto found = nEvnetsHBF.find(nevents);
//               if (found == nEvnetsHBF.end()) {
//                 nEvnetsHBF[nevents] = 1;
//               } else {
//                 found->second++;
//               }
//             } else {
//               LOG(debug) << "New HBF or Timeframe";
//               hbfbuffer.clear();
//               currentir.bc = o2::raw::RDHUtils::getTriggerBC(rdh);
//               currentir.orbit = o2::raw::RDHUtils::getTriggerOrbit(rdh);
//             }
//           } else {
//             LOG(error) << "Found unknown trigger" << std::bitset<32>(trigger);
//           }
//           currentpos += o2::raw::RDHUtils::getOffsetToNext(rdh);
//           continue;
//         }
//         if (o2::raw::RDHUtils::getStop(rdh)) {
//           LOG(error) << "Unexpected stop";
//         }

//         // non-0 payload size:
//         auto payloadsize = o2::raw::RDHUtils::getMemorySize(rdh) - o2::raw::RDHUtils::getHeaderSize(rdh);
//         int endpoint = static_cast<int>(o2::raw::RDHUtils::getEndPointID(rdh));
//         LOG(debug) << "Next RDH: ";
//         LOG(debug) << "Found endpoint              " << endpoint;
//         LOG(debug) << "Found trigger BC:           " << o2::raw::RDHUtils::getTriggerBC(rdh);
//         LOG(debug) << "Found trigger Oribt:        " << o2::raw::RDHUtils::getTriggerOrbit(rdh);
//         LOG(debug) << "Found payload size:         " << payloadsize;
//         LOG(debug) << "Found offset to next:       " << o2::raw::RDHUtils::getOffsetToNext(rdh);
//         LOG(debug) << "Stop bit:                   " << (o2::raw::RDHUtils::getStop(rdh) ? "yes" : "no");
//         auto page_payload = dataBuffer.subspan(currentpos + o2::raw::RDHUtils::getHeaderSize(rdh), payloadsize);
//         std::copy(page_payload.begin(), page_payload.end(), std::back_inserter(hbfbuffer));
//         currentpos += o2::raw::RDHUtils::getOffsetToNext(rdh);
//       }
//     }
//     reader.setNextTFToRead(++tfID);
//     nTFprocessed++;
//   }
//   rootfilewriter->Write();
//   LOG(info) << "Processed " << nTFprocessed << " timeframes, " << nHBFprocessed << " HBFs";
//   LOG(info) << "Analyzed " << nEventsProcessed << " events:";
//   LOG(info) << "=============================================================";
//   for (auto& [nevents, nHBF] : nEvnetsHBF) {
//     LOG(info) << "  " << nevents << " event(s)/HBF: " << nHBF << " HBFs ...";
//   }
// }