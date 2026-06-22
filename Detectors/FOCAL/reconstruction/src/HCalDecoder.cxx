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

#include <iostream>
#include <fstream>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

#include <gsl/span>

#include <fairlogger/Logger.h>
#include "FOCALReconstruction/HCalDecoder.h"

using namespace o2::focal;

void HCalDecoder::reset() {
  mHasData = false;
  LOGF(debug, "Resetting HCal decoder");
  LOGF(debug, "Line counters:     %d %d", mLinkLineCounters[0], mLinkLineCounters[1]);
  LOGF(debug, "Sample counters:   %d %d", mLinkSampleCounters[0], mLinkSampleCounters[1]);
  LOGF(debug, "Active frame flag: %d %d", mLinkFrameActive[0], mLinkFrameActive[1]);
  for (int sample = 0; sample < 16; ++sample) {
    for (int i = 0; i < 2; ++i) {
      mLinks[sample][i].reset();
      mLinkLineCounters[i] = 0;
      mLinkSampleCounters[i] = 0;
      mLinkFrameActive[i] = false;
    }
  }
}

bool HCalDecoder::isNullLine(HCalGBTLine ln) {
  return ( ln.words[0] | ln.words[1] | ln.words[2] | ln.words[3] |
           ln.words[4] | ln.words[5] | ln.words[6] | ln.words[7] ) == 0;
}

bool HCalDecoder::isIdleLine(HCalGBTLine ln) {
  return ( (ln.words[2] == 0xACCCCCCC) |
           (ln.words[3] == 0xACCCCCCC) |
           (ln.words[4] == 0xACCCCCCC) |
           (ln.words[5] == 0xACCCCCCC) ) == 1;
}

bool HCalDecoder::isTriggerLine(HCalGBTLine ln) {
  return ( (ln.words[0] == 0xBBBBBBBB) == 1 );
}

void HCalDecoder::decodeBuffer(gsl::span<const char> buffer) {
  if (buffer.size() == 0) {
    return;
  }

  LOGF(debug, "Decoding %d bytes", buffer.size());
  mHasData = true;
   
  // Cast the buffer to a vector of "lines" so we can easily iterate over them
  gsl::span<const HCalGBTLine> lines(reinterpret_cast<const HCalGBTLine*>(buffer.data()), buffer.size() / sizeof(HCalGBTLine));
  for (auto const line : lines) {

    // After a reset, we are looking for the first non-zero, non-idle, non-trigger line,
    // which marks the first data line of the first sample of the event.
    // Probably a good idea to add another check to see if we get the DAQH header and trailer patterns,
    // since it has been observed that bit flip / shift corruptions can cause idle words to not be recognized as such
    if ( isNullLine(line) | isIdleLine(line) | isTriggerLine(line) ) { 
      continue; 
    }

    int link_id = line.link_id();
    if (not mLinkFrameActive[link_id]) {
      mLinkFrameActive[link_id] = true;
      LOGF(debug, "--v-- Link %02d start of DAQ frame --v--", link_id);
    }

    LOGF(debug, "(L%02d, s%02d) %02X %02X %04X %08X %08X %08X %08X %08X %08X %08X", 
                                                        mLinkLineCounters[link_id] + 1,
                                                        mLinkSampleCounters[link_id] + 1,
                                                        line.hdr(), 
                                                        line.link_id(),  
                                                        line.bx_cntr(),  
                                                        line.ob_cntr(),  
                                                        line.words[2].data,  
                                                        line.words[3].data,  
                                                        line.words[4].data,  
                                                        line.words[5].data,
                                                        line.words[6].data,
                                                        line.words[7].data);

    if (mLinkFrameActive[link_id]) {

      // In rare cases, bit shift corruptions in data can result in an erroneus sample count,
      // so exit early to prevent segmentation faults
      if (mLinkSampleCounters[link_id] > 15) {
        LOGF(error, "Sample counter greater than number of samples! (%d)", mLinkSampleCounters[link_id]);
      	return;
      }
      
      mLinks[mLinkSampleCounters[link_id]][link_id].fillData(line, mLinkLineCounters[link_id]);     
      ++mLinkLineCounters[link_id];

      // 40 lines marks the end of a frame, always
      if (mLinkLineCounters[link_id] == 40) {
        LOGF(info, "--^-- Link %02d end of DAQ frame --^--", link_id);
        mLinkFrameActive[link_id] = false;
        mLinkLineCounters[link_id] = 0;
        ++mLinkSampleCounters[link_id];
      }
    }
  }
}
