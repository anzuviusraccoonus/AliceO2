// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
#ifndef ALICEO2_FOCAL_HCALDECODER_H
#define ALICEO2_FOCAL_HCALDECODER_H


#include <array>
#include <vector>
#include <functional>
#include <cstdint>

#include "Rtypes.h"

#include <gsl/span>

#include "FOCALReconstruction/HCalDataWord.h"
#include "FOCALReconstruction/HCalGBTLink.h"
 
namespace o2::focal
{

struct LinkContext {
  enum class State {
    WaitingForFrame,
    ReadingFrame,
    Finished,
    Error
  };

  State state   = State::WaitingForFrame;
  bool  gotL1A  = false;
  int   distL1A = 0;
  int   numL1A  = 0;
  int   samples = 0;
  int   lines   = 0;
  int   id      = -1;
};

class HCalDecoder {
 public:
  HCalDecoder();
  ~HCalDecoder();

  void reset();
  void decodeBuffer(gsl::span<const char> buffer);
  void processLine(HCalGBTLine line, LinkContext* ctx);

  bool isTriggerLine(HCalGBTLine line);
  bool isNullLine(HCalGBTLine line);
  bool isIdleLine(HCalGBTLine line);
  bool isDAQHLine(HCalGBTLine line);
  
  bool hasEventData() { return mHasData; }
  bool isDataValid()  { return mIsDataValid; }

  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> getData() { return mLinks; }

  // Per-event access
  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> getEventData(int eventIndex) { return mEvents.at(eventIndex); }
  int getNumEvents() const { return static_cast<int>(mEvents.size()); }
  int getNumSamplesRead(int link_id) { return mLinkContexts[link_id]->samples; }
  int getL1ADistance(int link_id) { return mLinkContexts[link_id]->distL1A; }
  int getNumL1ACmds(int link_id) { return mLinkContexts[link_id]->numL1A; }

 private:
  bool mIsDataValid;
  bool mHasData;

  // Per-HBF storage
  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> mLinks = {};
  LinkContext* mLinkContexts[constants::HCAL_NUM_GBT_LINKS];

  // Per-event storage (trigger line separates events)
  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> mLinksPerEv = {};
  std::vector<std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT>> mEvents = {};
  std::array<int, constants::HCAL_NUM_GBT_LINKS> mLinkLineCountersEv = {};
  std::array<int, constants::HCAL_NUM_GBT_LINKS> mLinkSampleCountersEv = {};
  std::array<bool, constants::HCAL_NUM_GBT_LINKS> mLinkFrameActiveEv = {};
  
  ClassDefNV(HCalDecoder, 1);
};

} // namespace o2::focal

#endif // ALICEO2_FOCAL_HCALDECODER_H
