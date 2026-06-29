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
  LOGF(debug, "Resetting HCal decoder");

  // TODO: put these attributes on each link or ROC?
  mHasData = false;
  mIsDataValid = true;

  for (int i = 0; i < constants::HCAL_NUM_GBT_LINKS; ++i) {
    mLinkContexts[i].state = LinkContext::State::WaitingForFrame;
    mLinkContexts[i].samples = 0;
    mLinkContexts[i].lines = 0;
    mLinkContexts[i].id = i;

    for (int sample = 0; sample < constants::HCAL_NUM_SAMPLES_PER_EVENT; ++sample) {
      mLinks[sample][i].reset();
      mLinksPerEv[sample][i].reset();
      mLinkLineCountersEv[i] = 0;
      mLinkSampleCountersEv[i] = 0;
      mLinkFrameActiveEv[i] = false;
    }
  }
  mEvents.clear();
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

bool HCalDecoder::isDAQHLine(HCalGBTLine ln) {
  constexpr unsigned int headerPattern = 0xF0000005;
  HCalDataWord dw0 = static_cast<HCalDataWord>(ln.words[2]);
  HCalDataWord dw1 = static_cast<HCalDataWord>(ln.words[3]);
  HCalDataWord dw2 = static_cast<HCalDataWord>(ln.words[4]);
  HCalDataWord dw3 = static_cast<HCalDataWord>(ln.words[5]);
  return ( ( (dw0 & headerPattern) == headerPattern) |
           ( (dw1 & headerPattern) == headerPattern) |
           ( (dw2 & headerPattern) == headerPattern) |
           ( (dw3 & headerPattern) == headerPattern) ) == 1;
}

void HCalDecoder::processLine(HCalGBTLine line, LinkContext& ctx) {
  switch (ctx.state) {
    case LinkContext::State::WaitingForFrame:
      LOGF(debug, "LinkContext %d is in state WaitingForFrame", ctx.id);
      if (isIdleLine(line)) {
        return;
      } else if (isDAQHLine(line)) {
        LOGF(debug, "LinkContext %d got DAQH line; state transition -> ReadingFrame", ctx.id);
        ctx.state = LinkContext::State::ReadingFrame;
      } else {
        LOGF(warn, "LinkContext %d expected IDLE or DAQH line but got neither - potential corruption in event", ctx.id);
        ctx.state = LinkContext::State::Error;
        return;
      }

    // Break/return statement intentionally left out here, such that
    // the state transition to ReadingFrame also reads the DAQH line

    case LinkContext::State::ReadingFrame:
      LOGF(debug, "LinkContext %d is in state ReadingFrame", ctx.id);

      // Sometimes we get a DAQH line that is followed up by an IDLE,
      // rarely with a small number of actual data lines first - not sure why
      if (isIdleLine(line)) {
        LOGF(warn, "LinkContext %d got IDLE line in a DAQ frame - sample data incomplete (%d lines read in sample %d)", ctx.id, ctx.lines, ctx.samples);
        ctx.lines = 0;
        if ((ctx.samples++)+1 == constants::HCAL_NUM_SAMPLES_PER_EVENT) {
          LOGF(debug, "LinkContext %d read %d samples; state transition -> Finished", ctx.id, ctx.samples);
          ctx.state = LinkContext::State::Finished;
        } else {
          ctx.state = LinkContext::State::WaitingForFrame;
        }

        break;
      }

      // This block is the normal execution path if everything is well
      LOGF(debug, "LinkContext %d filling data (sample %d, line %d)", ctx.id, ctx.samples, ctx.lines);
      mLinks[ctx.samples][ctx.id].fillData(line, ctx.lines);
      
      // Also accumulate per-event data in parallel
      int link_id = line.link_id();
      if (mLinkSampleCountersEv[link_id] <= 15) {
        mLinksPerEv[mLinkSampleCountersEv[link_id]][link_id].fillData(line, mLinkLineCountersEv[link_id]);
        ++mLinkLineCountersEv[link_id];
        if (mLinkLineCountersEv[link_id] == 40) {
          mLinkFrameActiveEv[link_id] = false;
          mLinkLineCountersEv[link_id] = 0;
          ++mLinkSampleCountersEv[link_id];
        }
      }
      
      if ((ctx.lines++)+1 == constants::HCAL_NUM_GBT_LINES_PER_LINK) {
        if ((ctx.samples++)+1 == constants::HCAL_NUM_SAMPLES_PER_EVENT) {
          LOGF(debug, "LinkContext %d read %d samples; state transition -> Finished", ctx.id, ctx.samples);
          ctx.state = LinkContext::State::Finished;
        } else {
          LOGF(debug, "LinkContext %d read %d lines; state transition -> WaitingForFrame", ctx.id, ctx.lines);
          ctx.lines = 0;
          ctx.state = LinkContext::State::WaitingForFrame;
        }
      }

      return;

    case LinkContext::State::Finished:
      LOGF(debug, "LinkContext %d is in state Finished", ctx.id);
      return;

    case LinkContext::State::Error:
      LOGF(debug, "LinkContext %d is in state Error", ctx.id);
      return;
      
    default:
      LOGF(error, "LinkContext %d is in an unknown state! Samples read: %d    Lines read: %d", ctx.id, ctx.samples, ctx.lines);
      return;
  }

}

void HCalDecoder::decodeBuffer(gsl::span<const char> buffer) {
  if (buffer.size() == 0) {
    return;
  }

  LOGF(debug, "Decoding %d bytes", buffer.size());

  // Cast the buffer to a vector of "lines" so we can easily iterate over them
  gsl::span<const HCalGBTLine> lines(reinterpret_cast<const HCalGBTLine*>(buffer.data()), buffer.size() / sizeof(HCalGBTLine));
  for (const HCalGBTLine& line : lines) {
    // Skip decoding padded zeroes, and also the trigger line
    if (isNullLine(line)) {
      continue;
    }
    
    LOGF(debug, "%04X %08X %08X %08X %08X %08X %08X %08X", 
         line.hdr(), line.link_id(), line.bx_cntr(), line.ob_cntr(), 
         line.words[2].data, line.words[3].data, line.words[4].data,  
         line.words[5].data, line.words[6].data, line.words[7].data
         );
    
    // Trigger line marks the boundary between events: flush accumulated per-event data
    if (isTriggerLine(line)) {
      if (mLinkSampleCountersEv[0] > 0 || mLinkSampleCountersEv[1] > 0) {
        mEvents.push_back(mLinksPerEv);
        for (int i = 0; i < constants::HCAL_NUM_GBT_LINKS; ++i) {
          for (int j = 0; j < constants::HCAL_NUM_SAMPLES_PER_EVENT; ++j) {
            mLinksPerEv[j][i].reset();
          }
          mLinkLineCountersEv[i] = 0;
          mLinkSampleCountersEv[i] = 0;
          mLinkFrameActiveEv[i] = false;
        }
      }
      continue;
    }

    int link_id = line.link_id();
    processLine(line, mLinkContexts[link_id]);
  }

  LOGF(debug, "Decoding finished");
  for (LinkContext& ctx : mLinkContexts) {
    // If the data on any link wasn't readable, set this flag so
    // other tasks can know to discard this event, if wanted
    // TODO: set this flag on a per-link or per-chip basis?
    if (ctx.state == LinkContext::State::Error) {
      mIsDataValid = false;
    }

    // If a link context is still in the initial state, it means
    // that the entirety of this payload was empty
    // TODO: set this flag on a per-link or per-chip basis?
    if (not (ctx.state == LinkContext::State::WaitingForFrame)) {
      mHasData = true;
    }
  }

  // Flush the last in-progress event (final event in HBF has no trailing trigger line)
  if (mLinkSampleCountersEv[0] > 0 || mLinkSampleCountersEv[1] > 0) {
    mEvents.push_back(mLinksPerEv);
  }
}
