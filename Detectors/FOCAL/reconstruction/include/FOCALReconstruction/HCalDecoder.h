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

class HCalDecoder {
 public:
  HCalDecoder() = default;
  ~HCalDecoder() = default;

  void setTriggerWinDur(int windur) { mWin_dur = windur; } // unused?

  void reset();
  void decodeBuffer(gsl::span<const char> buffer);

  bool isNullLine(HCalGBTLine line);
  bool isIdleLine(HCalGBTLine line);
  bool isTriggerLine(HCalGBTLine line);
  bool hasEventData() { return mHasData; }
  std::array<int, constants::HCAL_NUM_GBT_LINKS> getNumSamplesRead() { return mLinkSampleCounters; }
  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> getData() { return mLinks; }

 private:
  int mWin_dur = 20; // unused?
  bool mHasData;
  std::array<std::array<HCalGBTLink, constants::HCAL_NUM_GBT_LINKS>, constants::HCAL_NUM_SAMPLES_PER_EVENT> mLinks = {};
  std::array<int, constants::HCAL_NUM_GBT_LINKS> mLinkLineCounters = {};
  std::array<int, constants::HCAL_NUM_GBT_LINKS> mLinkSampleCounters = {};
  std::array<bool, constants::HCAL_NUM_GBT_LINKS> mLinkFrameActive = {};

  ClassDefNV(HCalDecoder, 1);
};

} // namespace o2::focal

#endif // ALICEO2_FOCAL_HCALDECODER_H
